#pragma once

#include "HarvestableListReader.hpp"
#include "KinematicsListReader.hpp"
#include "BlueprintListReader.hpp"
#include "AssetListReader.hpp"
#include "DecalListReader.hpp"
#include "ClutterReader.hpp"
#include "PrefabReader.hpp"
#include "MipReader.hpp"

#include "Converter/TileConverter/CellHeader.hpp"
#include "Converter/TileConverter/TileHeader.hpp"
#include "Converter/TileConverter/Tile.hpp"

#include "ObjectDatabase/ProgCounter.hpp"
#include "UStd/UnmanagedString.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/String.hpp"
#include "Utils/File.hpp"

SM_UNMANAGED_CODE

class TileReader
{
	TileReader() = default;

public:
	static bool ReadTileHeader(const std::wstring& path, TileHeaderBaseInfo& out_header, ConvertError& error)
	{
		std::ifstream v_tileFile(path, std::ios::binary);
		if (!v_tileFile.is_open())
		{
			error.setError(1, L"TileReader::ReadTile -> Couldn't open the file");
			return false;
		}

		struct
		{
			std::uint32_t secret;
			TileHeaderBaseInfo info;
		} v_tileHeader;

		if (!v_tileFile.read(reinterpret_cast<char*>(&v_tileHeader), sizeof(v_tileHeader)).good())
		{
			error.setError(1, L"TileReader::ReadTile -> Couldn't read the file");
			return false;
		}

		if (v_tileHeader.secret != 0x454C4954) //TILE - magic keyword
		{
			DebugOutL("Invalid File");
			error.setError(1, L"TileHeader::ReadTile -> Invalid File");
			return false;
		}

		if (v_tileHeader.info.version > 1000000)
		{
			DebugErrorL("Invalid Version");
			error.setError(1, L"TileHeader::ReadTile -> Invalid Tile Version");
			return false;
		}

		if (v_tileHeader.info.cell_header_offset != sizeof(v_tileHeader))
		{
			error.setError(1, L"TileHeader::ReadTile -> Index doesn't match the cell header offset!");
			return false;
		}

		out_header = v_tileHeader.info;
		return true;
	}

	template<bool t_mod_counter>
	static bool ReadTile(const std::wstring& path, ConvertError& error, Tile& out_tile)
	{
		std::vector<Byte> v_fileBytes;
		if (!File::ReadFileBytes(path, v_fileBytes))
			return false;

		return TileReader::ReadTile<t_mod_counter>(v_fileBytes, error, out_tile);
	}

	template<bool t_mod_counter>
	inline static void ReadTileData(
		const CellHeader& header,
		MemoryWrapper& reader,
		TilePart* pPart,
		int version,
		ConvertError& error)
	{
		AssetListReader::Read<t_mod_counter>      (header, reader, pPart, version, error);
		PrefabReader::Read<t_mod_counter>         (header, reader, pPart, version, error);
		BlueprintListReader::Read<t_mod_counter>  (header, reader, pPart, version, error);
		HarvestableListReader::Read<t_mod_counter>(header, reader, pPart, version, error);
		DecalListReader::Read<t_mod_counter>      (header, reader, pPart, version, error);

		if (version >= 11)
			KinematicsListReader::Read<t_mod_counter>(header, reader, pPart, version, error);
	}

	template<bool t_mod_counter>
	static bool ReadTile(const std::vector<Byte>& tileData, ConvertError& error, Tile& out_tile)
	{
		TileHeader v_header;
		if (!TileHeader::ReadTile(&v_header, tileData, error))
			return false;

		DebugOutL("TileFileVersion: ", v_header.m_data.version);
		DebugOutL("TileUuid: ", v_header.m_data.uuid.ToString());
		DebugOutL("CreatorId: ", v_header.m_data.creator_id);
		DebugOutL("Size: ", v_header.m_data.width, ", ", v_header.m_data.height);
		DebugOutL("Type: ", v_header.m_data.type, "\n");
		DebugOutL("Header info:");
		DebugOutL("CellHeaderOffset: ", v_header.m_data.cell_header_offset);
		DebugOutL("CellHeadersSize: ", v_header.m_data.cell_header_size, "\n");

		const int v_tileVersion = v_header.m_data.version;
		if (v_tileVersion < 0 || v_tileVersion > 13)
		{
			error.setError(1, L"Unsupported Tile Version: " + std::to_wstring(v_tileVersion));
			return false;
		}

		if (v_header.m_data.width != v_header.m_data.height)
		{
			error.setError(1, L"Weird tile dimensions: " + std::to_wstring(v_header.m_data.width) + L", " + std::to_wstring(v_header.m_data.height));
			return false;
		}

		MemoryWrapper v_reader(tileData);
		Tile v_newTile(v_header);

		for (int y = 0; y < v_header.m_data.height; y++)
		{
			for (int x = 0; x < v_header.m_data.width; x++)
			{
				const CellHeader& v_cellHeader = v_header.GetCellHeader(x, y);
				TilePart* v_pPart = v_newTile.GetPart(x, y);

				if constexpr (!t_mod_counter) //mod counter doesn't need any information about the clutter, so it is skipped to improve performance
				{
					if (v_newTile.m_hasTerrain)
					{
						MipReader::Read(v_cellHeader, v_reader, v_pPart, error);
						if (error) goto escape_loop;

						ClutterReader::Read(v_cellHeader, v_reader, v_pPart, error);
						if (error) goto escape_loop;
					}
				}

				TileReader::ReadTileData<t_mod_counter>(v_cellHeader, v_reader, v_pPart, v_tileVersion, error);
				if (error) goto escape_loop;
			}
		}

	escape_loop:
		if (error)
			return false;

		out_tile = std::move(v_newTile);
		return true;
	}
};

SM_MANAGED_CODE
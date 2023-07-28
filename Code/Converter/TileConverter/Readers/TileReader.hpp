#pragma once

#include "HarvestableListReader.hpp"
#include "KinematicsListReader.hpp"
#include "BlueprintListReader.hpp"
#include "AssetListReader.hpp"
#include "DecalListReader.hpp"
#include "ClutterReader.hpp"
#include "PrefabReader.hpp"
#include "MipReader.hpp"

#include "Converter\TileConverter\CellHeader.hpp"
#include "Converter\TileConverter\TileHeader.hpp"
#include "Converter\TileConverter\Tile.hpp"

#include "ObjectDatabase\ProgCounter.hpp"
#include "UStd\UnmanagedString.hpp"

#include "Utils\String.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

class TileReader
{
	TileReader() = default;

public:
	static bool ReadTileHeader(const std::wstring& path, TileHeaderBaseInfo& v_header, ConvertError& cError)
	{
		std::vector<Byte> v_file_bytes;
		if (!File::ReadFileBytes(path, v_file_bytes))
		{
			cError = ConvertError(1, L"TileReader::ReadTile -> Couldn't read the file");
			return false;
		}

		MemoryWrapper v_memory = v_file_bytes;

		const int v_tile_key = v_memory.NextObject<int>();
		if (v_tile_key != 0x454C4954) //TILE - magic keyword
		{
			DebugOutL("Invalid File");
			cError = ConvertError(1, L"TileHeader::ReadTile -> Invalid File");
			return false;
		}

		v_memory.NextObjectRef<TileHeaderBaseInfo>(&v_header);
		if (v_header.version > 1000000)
		{
			DebugErrorL("Invalid Version");
			cError = ConvertError(1, L"TileHeader::ReadTile -> Invalid Tile Version");
			return false;
		}

		if (v_memory.Index() != v_header.cell_header_offset)
		{
			cError = ConvertError(1, L"TileHeader::ReadTile -> Index doesn't match the cell header offset!");
			return false;
		}

		return true;
	}

	template<bool t_mod_counter>
	static Tile* ReadTile(const std::wstring& path, ConvertError& cError)
	{
		std::vector<Byte> v_file_bytes;
		if (!File::ReadFileBytes(path, v_file_bytes))
			return nullptr;

		return TileReader::ReadTile<t_mod_counter>(v_file_bytes, cError);
	}

	template<bool t_mod_counter, int t_tile_version>
	inline static void ReadTileData(CellHeader* v_header, MemoryWrapper& v_reader, TilePart* v_part, ConvertError& v_error)
	{
		AssetListReader::Read<t_mod_counter, t_tile_version>      (v_header, v_reader, v_part, v_error);
		PrefabReader::Read<t_mod_counter, t_tile_version>         (v_header, v_reader, v_part, v_error);
		BlueprintListReader::Read<t_mod_counter, t_tile_version>  (v_header, v_reader, v_part, v_error);
		HarvestableListReader::Read<t_mod_counter, t_tile_version>(v_header, v_reader, v_part, v_error);
		DecalListReader::Read<t_mod_counter, t_tile_version>      (v_header, v_reader, v_part, v_error);

		if constexpr (t_tile_version >= 11) {
			KinematicsListReader::Read<t_mod_counter, t_tile_version> (v_header, v_reader, v_part, v_error);
		}
	}

	template<bool t_mod_counter>
	static Tile* ReadTile(const std::vector<Byte>& tile_data, ConvertError& v_error)
	{
		TileHeader header;
		if (!TileHeader::ReadTile(&header, tile_data, v_error))
			return nullptr;

	#if defined(DEBUG) || defined(_DEBUG)
		DebugOutL("TileFileVersion: ", header.m_data.version);
		DebugOutL("TileUuid: ", header.m_data.uuid.ToString());
		DebugOutL("CreatorId: ", header.m_data.creator_id);
		DebugOutL("Size: ", header.m_data.width, ", ", header.m_data.height);
		DebugOutL("Type: ", header.m_data.type, "\n");
		DebugOutL("Header info:");
		DebugOutL("CellHeaderOffset: ", header.m_data.cell_header_offset);
		DebugOutL("CellHeadersSize: ", header.m_data.cell_header_size, "\n");
	#endif

		using u_tile_loader_function = void (*)(CellHeader*, MemoryWrapper&, TilePart*, ConvertError&);
		const static u_tile_loader_function v_tile_loaders[] =
		{
			TileReader::ReadTileData<t_mod_counter, 1>,
			TileReader::ReadTileData<t_mod_counter, 2>,
			TileReader::ReadTileData<t_mod_counter, 3>,
			TileReader::ReadTileData<t_mod_counter, 4>,
			TileReader::ReadTileData<t_mod_counter, 5>,
			TileReader::ReadTileData<t_mod_counter, 6>,
			TileReader::ReadTileData<t_mod_counter, 7>,
			TileReader::ReadTileData<t_mod_counter, 8>,
			TileReader::ReadTileData<t_mod_counter, 9>,
			TileReader::ReadTileData<t_mod_counter, 10>,
			TileReader::ReadTileData<t_mod_counter, 11>,
			TileReader::ReadTileData<t_mod_counter, 12>,
			TileReader::ReadTileData<t_mod_counter, 13>
		};

		constexpr const int v_numSupportedVersions = sizeof(v_tile_loaders) / sizeof(u_tile_loader_function);

		const int v_tileVersion = header.m_data.version;
		if (v_tileVersion < 0 || v_tileVersion > v_numSupportedVersions)
		{
			v_error = ConvertError(1, L"Unsupported Tile Version: " + std::to_wstring(v_tileVersion));
			return nullptr;
		}

		if (header.m_data.width != header.m_data.height)
		{
			v_error = ConvertError(1, L"Weird tile dimensions: " + std::to_wstring(header.m_data.width) + L", " + std::to_wstring(header.m_data.height));
			return nullptr;
		}

		MemoryWrapper reader(tile_data);

		const int tileXSize = header.m_data.width;
		const int tileYSize = header.m_data.height;

		Tile* tile = new Tile(header);
		if (tileYSize > 0)
		{
			u_tile_loader_function v_current_loader = v_tile_loaders[v_tileVersion - 1];
			for (int y = 0; y < tileYSize; y++)
			{
				if (v_error) break;

				for (int x = 0; x < tileXSize; x++)
				{
					CellHeader* h  = header.GetCellHeader(x, y);
					TilePart* part = tile->GetPart(x, y);

					if constexpr (!t_mod_counter) //mod counter doesn't need any information about the clutter, so it is skipped to improve performance
					{
						if (tile->m_hasTerrain)
						{
							MipReader::Read(h, reader, part, v_error);
							ClutterReader::Read(h, reader, part, v_error);
						}
					}
				
					v_current_loader(h, reader, part, v_error);
				}
			}
		}

		if (v_error)
		{
			delete tile;
			return nullptr;
		}

		return tile;
	}
};

#pragma managed
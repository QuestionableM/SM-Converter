#pragma once

#include "HarvestableListReader.hpp"
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

		MemoryWrapper mMemory = v_file_bytes;

		const std::string v_tile_key = mMemory.NextString(4);
		if (v_tile_key != "TILE")
		{
			cError = ConvertError(1, L"TileReader::ReadTile -> Invalid File");
			return false;
		}

		v_header.version = mMemory.NextObject<int>();

		if (v_header.version <= 1000000)
		{
			v_header.uuid = mMemory.NextObject<SMUuid>();
			v_header.creator_id = mMemory.NextObject<long long>();
		}

		v_header.width = mMemory.NextObject<int>();
		v_header.height = mMemory.NextObject<int>();

		v_header.cell_header_offset = mMemory.NextObject<int>();
		v_header.cell_header_size = mMemory.NextObject<int>();
		mMemory.Skip(8);

		if (v_header.version <= 1000000)
		{
			v_header.type = mMemory.NextObject<int>() >> 0x18;
		}

		if (mMemory.Index() != v_header.cell_header_offset)
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
	}

	template<bool t_mod_counter>
	static Tile* ReadTile(const std::vector<Byte>& tile_data, ConvertError& v_error)
	{
		ProgCounter::SetState(ProgState::ReadingTile, 0);

		TileHeader* header = TileHeader::ReadTile(tile_data, v_error);
		if (!header) return nullptr;

	#if defined(DEBUG) || defined(_DEBUG)
		DebugOutL("TileFileVersion: ", header->m_data.version);
		DebugOutL("TileUuid: ", header->m_data.uuid.ToString());
		DebugOutL("CreatorId: ", header->m_data.creator_id);
		DebugOutL("Size: ", header->m_data.width, ", ", header->m_data.height);
		DebugOutL("Type: ", header->m_data.type, "\n");
		DebugOutL("Header info:");
		DebugOutL("CellHeaderOffset: ", header->m_data.cell_header_offset);
		DebugOutL("CellHeadersSize: ", header->m_data.cell_header_size, "\n");

		for (int a = 0; a < header->m_data.width * header->m_data.height; a++)
		{
			int x = a % header->m_data.width;
			int y = a / header->m_data.width;

			std::vector<Byte> bytes = header->GetHeader(x, y)->Data();
			DebugOutL("\tBLOB(", x, ", ", y, "):");
			DebugOutL("\t\t", String::BytesToHexString(bytes, header->m_data.cell_header_size, 32));
		}
	#endif

		const int v_tileVersion = header->m_data.version;
		if (v_tileVersion < 0 || v_tileVersion > 13)
		{
			v_error = ConvertError(1, L"Unsupported Tile Version: " + std::to_wstring(v_tileVersion));
			return nullptr;
		}

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

		MemoryWrapper reader(header->TileData());

		const int tileXSize = header->m_data.width;
		const int tileYSize = header->m_data.width;

		Tile* tile = new Tile(tileXSize, tileYSize);
		tile->SetVersion(header->m_data.version);
		tile->SetTileType(header->m_data.type);
		tile->SetCreatorId(header->m_data.creator_id);

		if (tileYSize > 0)
		{
			u_tile_loader_function v_current_loader = v_tile_loaders[v_tileVersion - 1];
			for (int y = 0; y < tileYSize; y++)
			{
				if (v_error) break;

				for (int x = 0; x < tileXSize; x++)
				{
					CellHeader* h  = header->GetHeader(x, y);
					TilePart* part = tile->GetPart(x, y);

					if constexpr (!t_mod_counter) //mod counter doesn't need any information about the clutter, so it is skipped to improve performance
					{
						if (header->m_data.type == 0)
						{
							MipReader::Read(h, reader, part, v_error);
							ClutterReader::Read(h, reader, part, v_error);
						}
					}
				
					v_current_loader(h, reader, part, v_error);
				}
			}
		}

		delete header;

		if (v_error)
		{
			delete tile;
			return nullptr;
		}

		return tile;
	}
};

#pragma managed
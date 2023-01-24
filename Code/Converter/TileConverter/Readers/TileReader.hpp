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
		std::vector<std::uint8_t> v_file_bytes = File::ReadFileBytes(path);
		if (v_file_bytes.empty())
		{
			cError = ConvertError(1, L"TileReader::ReadTile -> Couldn't read the file");
			return false;
		}

		MemoryWrapper mMemory = v_file_bytes;

		std::vector<char> v_tile_keyword = mMemory.NextObjects<char>(4);
		std::string v_tile_key(v_tile_keyword.begin(), v_tile_keyword.end());

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
		std::vector<Byte> file_bytes = File::ReadFileBytes(path);

		return TileReader::ReadTile<t_mod_counter>(file_bytes, cError);
	}

	template<bool t_mod_counter>
	static Tile* ReadTile(const std::vector<Byte>& tile_data, ConvertError& cError)
	{
		ProgCounter::SetState(ProgState::ReadingTile, 0);

		TileHeader* header = TileHeader::ReadTile(tile_data, cError);
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

		MemoryWrapper reader(header->TileData());

		const int tileXSize = header->m_data.width;
		const int tileYSize = header->m_data.width;

		Tile* tile = new Tile(tileXSize, tileYSize);
		tile->SetVersion(header->m_data.version);
		tile->SetTileType(header->m_data.type);
		tile->SetCreatorId(header->m_data.creator_id);

		if (tileYSize > 0)
		{
			for (int y = 0; y < tileYSize; y++)
			{
				if (cError) break;

				for (int x = 0; x < tileXSize; x++)
				{
					CellHeader* h  = header->GetHeader(x, y);
					TilePart* part = tile->GetPart(x, y);

					if (header->m_data.type == 0)
					{
						MipReader::Read    (h, reader, part, cError);
						ClutterReader::Read(h, reader, part, cError);
					}
				
					AssetListReader::Read<t_mod_counter>(h, reader, part, cError);
					PrefabReader::Read         (h, reader, part, cError);
					BlueprintListReader::Read  (h, reader, part, cError);
					HarvestableListReader::Read(h, reader, part, cError);
					DecalListReader::Read      (h, reader, part, cError);
				}
			}
		}

		delete header;

		if (cError)
		{
			delete tile;
			return nullptr;
		}

		return tile;
	}
};

#pragma managed
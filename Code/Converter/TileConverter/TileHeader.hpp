#pragma once

#include "Utils\ByteImpl.hpp"
#include "Utils\Console.hpp"
#include "Utils\Memory.hpp"
#include "Utils\Uuid.hpp"

#include "Converter\TileConverter\CellHeader.hpp"
#include "Converter\ConvertError.hpp"

#pragma unmanaged

#pragma pack(push, 1)
struct TileHeaderBaseInfo
{
	int version;
	SMUuid uuid;
	long long creator_id;

	int width;
	int height;

	int cell_header_offset;
	int cell_header_size;

	int some_val1;
	int some_val2;

	int type;
};
#pragma pack(pop)

class TileHeader
{
public:
	TileHeaderBaseInfo m_data;
	std::vector<CellHeader*> m_CellHeaders = {};

	TileHeader() = default;
	~TileHeader()
	{
		for (std::size_t a = 0; a < m_CellHeaders.size(); a++)
			delete m_CellHeaders[a];
	}

	static bool ReadTile(TileHeader* tile_header, const std::vector<Byte>& bytes, ConvertError& cError)
	{
		MemoryWrapper mMemory = bytes;

		const int v_tile_key = mMemory.NextObject<int>();
		if (v_tile_key != 0x454C4954) //TILE - magic keyword
		{
			DebugOutL("Invalid File");
			cError = ConvertError(1, L"TileHeader::ReadTile -> Invalid File");
			return false;
		}

		mMemory.NextObjectRef<TileHeaderBaseInfo>(&tile_header->m_data);
		if (tile_header->m_data.version > 1000000)
		{
			DebugErrorL("Invalid version");
			cError = ConvertError(1, L"TileHeader::ReadTile -> Invalid Tile Version");
			return false;
		}

		DebugOutL("Version: ", tile_header->m_data.version);
		DebugOutL("CreatorId: ", tile_header->m_data.creator_id);
		DebugOutL("Uuid: ", tile_header->m_data.uuid.ToString());
		DebugOutL("Size: {w: ", tile_header->m_data.width, ", h: ", tile_header->m_data.height, "}");
		DebugOutL("CellHeadersOffset: ", tile_header->m_data.cell_header_offset);
		DebugOutL("CellHeadersSize: ", tile_header->m_data.cell_header_size);
		DebugOutL("Type: ", tile_header->m_data.type);

		if (mMemory.Index() != tile_header->m_data.cell_header_offset)
		{
			DebugOutL("Error: index doesn't match the cell header offset!");
			cError = ConvertError(1, L"TileHeader::ReadTile -> Index doesn't match the cell header offset!");
			return false;
		}

		const int wh_mul = tile_header->m_data.width * tile_header->m_data.height;
		if (wh_mul != 0)
		{
			tile_header->m_CellHeaders.resize(wh_mul);

			for (int a = 0; a < wh_mul; a++)
			{
				tile_header->m_CellHeaders[a] = new CellHeader(mMemory.DataPtr(), tile_header->m_data.cell_header_size);
				mMemory.Skip(tile_header->m_data.cell_header_size);
			}
		}

		return true;
	}

	CellHeader* GetCellHeader(int x, int y)
	{
		return m_CellHeaders[(std::size_t)(x + y * m_data.width)];
	}
};

#pragma managed
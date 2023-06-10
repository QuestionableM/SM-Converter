#pragma once

#include "Utils\ByteImpl.hpp"
#include "Utils\Console.hpp"
#include "Utils\Memory.hpp"
#include "Utils\Uuid.hpp"

#include "Converter\TileConverter\CellHeader.hpp"
#include "Converter\ConvertError.hpp"

#pragma unmanaged

struct TileHeaderBaseInfo
{
	int version;
	long long creator_id;
	SMUuid uuid;

	int width;
	int height;
	int type;

	int cell_header_offset;
	int cell_header_size;
};

class TileHeader
{
	TileHeader() = default;

	std::vector<Byte> m_TileBytes = {};

public:
	TileHeaderBaseInfo m_data;
	std::vector<CellHeader*> m_CellHeaders = {};

	~TileHeader()
	{
		for (std::size_t a = 0; a < m_CellHeaders.size(); a++)
			delete m_CellHeaders[a];
	}

	void FillHeaderBytes(const std::vector<Byte>& header_bytes, std::size_t header_size)
	{
		const int wh_mul = m_data.width * m_data.height;
		for (int a = 0; a < wh_mul; a++)
		{
			std::vector<Byte> bytes_temp = {};
			bytes_temp.resize(header_size);

			std::memcpy(bytes_temp.data(), header_bytes.data() + (a * header_size), header_size);

			CellHeader* p_Header = new CellHeader(bytes_temp);
			p_Header->Read();

			this->m_CellHeaders[a] = p_Header;
		}
	}

	static TileHeader* ReadTile(const std::vector<Byte>& bytes, ConvertError& cError)
	{
		MemoryWrapper mMemory = bytes;

		const std::string v_tile_key = mMemory.NextString(4);
		if (v_tile_key != "TILE")
		{
			DebugOutL("Invalid File");
			cError = ConvertError(1, L"TileHeader::ReadTile -> Invalid File");
			return nullptr;
		}

		TileHeader* new_tile = new TileHeader();
		new_tile->m_TileBytes = bytes;

		new_tile->m_data.version = mMemory.NextObject<int>();
		DebugOutL("Version: ", new_tile->m_data.version);

		if (new_tile->m_data.version <= 1000000)
		{
			new_tile->m_data.uuid = mMemory.NextObject<SMUuid>();
			new_tile->m_data.creator_id = mMemory.NextObject<long long>();
		}

		DebugOutL("CreatorId: ", new_tile->m_data.creator_id);
		DebugOutL("Uuid: ", new_tile->m_data.uuid.ToString());

		new_tile->m_data.width  = mMemory.NextObject<int>();
		new_tile->m_data.height = mMemory.NextObject<int>();

		const int wh_mul = new_tile->m_data.width * new_tile->m_data.height;
		new_tile->m_CellHeaders.resize(wh_mul);

		DebugOutL("Size: {w: ", new_tile->m_data.width, ", h: ", new_tile->m_data.height, "}");

		new_tile->m_data.cell_header_offset = mMemory.NextObject<int>();
		new_tile->m_data.cell_header_size   = mMemory.NextObject<int>();
		mMemory.Skip(8);

		DebugOutL("CellHeadersOffset: ", new_tile->m_data.cell_header_offset);
		DebugOutL("CellHeadersSize: ", new_tile->m_data.cell_header_size);

		if (new_tile->m_data.version <= 1000000)
		{
			new_tile->m_data.type = mMemory.NextObject<int>() >> 0x18;
			DebugOutL("Type: ", new_tile->m_data.type);
		}

		if (mMemory.Index() != new_tile->m_data.cell_header_offset)
		{
			DebugOutL("Error: index doesn't match the cell header offset!");
			cError = ConvertError(1, L"TileHeader::ReadTile -> Index doesn't match the cell header offset!");

			delete new_tile;
			return nullptr;
		}

		if (wh_mul != 0)
		{
			//Keep an eye on this line of code, in case the reader breaks
			//388 bytes : 292 bytes
			const std::size_t header_size = (new_tile->m_data.version > 9) ? 0x184 : 0x124;

			std::vector<Byte> headerBytes = {};
			headerBytes.resize(wh_mul * header_size); //0x124

			for (int a = 0; a < wh_mul; a++)
				mMemory.NextObjectsRef(headerBytes.data() + (a * header_size), new_tile->m_data.cell_header_size);

			new_tile->FillHeaderBytes(headerBytes, header_size);
		}

		return new_tile;
	}

	const std::vector<Byte>& TileData() const noexcept
	{
		return m_TileBytes;
	}

	CellHeader* GetHeader(int x, int y)
	{
		return m_CellHeaders[(std::size_t)(x + y * m_data.width)];
	}
};

#pragma managed
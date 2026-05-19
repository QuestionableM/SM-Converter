#include "TileHeader.hpp"

SM_UNMANAGED_CODE

TileHeader::TileHeader()
	: m_data()
	, m_cellHeaders()
{}

bool TileHeader::ReadTile(
	TileHeader* pHeader,
	const std::vector<Byte>& bytes,
	ConvertError& error)
{
	MemoryWrapper v_memory = bytes;

	const std::uint32_t v_tileKey = v_memory.NextObject<std::uint32_t>();
	if (v_tileKey != 0x454C4954) //TILE - magic keyword
	{
		DebugOutL("Invalid File");
		error.setError(1, "TileHeader::ReadTile -> Invalid File");
		return false;
	}

	v_memory.NextObjectRef<TileHeaderBaseInfo>(&pHeader->m_data);
	if (pHeader->m_data.version > 1000000)
	{
		DebugErrorL("Invalid version");
		error.setError(1, "TileHeader::ReadTile -> Invalid Tile Version");
		return false;
	}

	DebugOutL("Version: ", pHeader->m_data.version);
	DebugOutL("CreatorId: ", pHeader->m_data.creator_id);
	DebugOutL("Uuid: ", pHeader->m_data.uuid.toString());
	DebugOutL("Size: {w: ", pHeader->m_data.width, ", h: ", pHeader->m_data.height, "}");
	DebugOutL("CellHeadersOffset: ", pHeader->m_data.cell_header_offset);
	DebugOutL("CellHeadersSize: ", pHeader->m_data.cell_header_size);
	DebugOutL("Type: ", pHeader->m_data.type);

	if (v_memory.Index() != pHeader->m_data.cell_header_offset)
	{
		DebugOutL("Error: index doesn't match the cell header offset!");
		error.setError(1, "TileHeader::ReadTile -> Index doesn't match the cell header offset!");
		return false;
	}

	const std::uint32_t v_headerCount = pHeader->m_data.width * pHeader->m_data.height;
	for (std::uint32_t a = 0; a < v_headerCount; a++)
	{
		pHeader->m_cellHeaders.emplace_back(v_memory.DataPtr(), pHeader->m_data.cell_header_size);
		v_memory.Skip(pHeader->m_data.cell_header_size);
	}

	return true;
}

const CellHeader& TileHeader::GetCellHeader(const int x, const int y) const noexcept
{
	return m_cellHeaders[std::size_t(x + y * m_data.width)];
}
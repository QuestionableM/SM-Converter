#pragma once

#include "Utils/clr_include.hpp"
#include "Utils/ByteImpl.hpp"
#include "Utils/Console.hpp"
#include "Utils/Memory.hpp"
#include "Utils/Uuid.hpp"

#include "Converter/TileConverter/CellHeader.hpp"
#include "Converter/ConvertError.hpp"

SM_UNMANAGED_CODE

#pragma pack(push, 1)
struct TileHeaderBaseInfo
{
	std::uint32_t version;
	SMUuid uuid;
	std::uint64_t creator_id;

	std::uint32_t width;
	std::uint32_t height;

	std::uint32_t cell_header_offset;
	std::uint32_t cell_header_size;

	int some_val1;
	int some_val2;

	int type;
};
#pragma pack(pop)

class TileHeader
{
public:
	TileHeader();

	static bool ReadTile(TileHeader* pHeader, const std::vector<Byte>& bytes, ConvertError& error);
	const CellHeader& GetCellHeader(const int x, const int y) const noexcept;

public:
	TileHeaderBaseInfo m_data;
	std::vector<CellHeader> m_cellHeaders;
};

SM_MANAGED_CODE
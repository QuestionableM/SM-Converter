#pragma once

#include "Converter/TileConverter/TileConverterContext.hpp"
#include "Converter/TileConverter/CellHeader.hpp"
#include "Converter/TileConverter/TilePart.hpp"

#include "UStd/UnmanagedSstream.hpp"
#include "UStd/UnmanagedIomanip.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/ByteImpl.hpp"
#include "Utils/Memory.hpp"
#include "Utils/Lz4Lib.hpp"

SM_UNMANAGED_CODE

class MipReader
{
	MipReader() = default;

public:
	static void Read(TileReaderContext& context);

	static bool Read(
		TileReaderContext& context,
		int mipOrLevel,
		std::vector<Byte>& outBytes);

	static void Read(
		const std::vector<Byte>& bytes,
		TilePart* pPart);
};

SM_MANAGED_CODE
#pragma once

#include "Converter/TileConverter/TileConverterContext.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/ByteImpl.hpp"

SM_UNMANAGED_CODE

#include <vector>

class DecalListReader
{
	DecalListReader() = default;

public:
	static void Read(TileReaderContext& context);

	static int Read(
		TileReaderContext& context,
		const std::vector<Byte>& bytes,
		int decal_count);
};

SM_MANAGED_CODE
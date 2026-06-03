#pragma once

#include "Converter/TileConverter/TileConverterContext.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/ByteImpl.hpp"

SM_UNMANAGED_CODE

#include <vector>

class HarvestableListReader
{
	HarvestableListReader() = default;

public:
	static void Read(TileReaderContext& context);

	static int Read(
		TileReaderContext& context,
		const std::vector<Byte>& bytes,
		int hvs_index,
		int len);
};

SM_MANAGED_CODE
#pragma once

#include "Converter/TileConverter/TileConverterContext.hpp"
#include "Converter/TileConverter/CellHeader.hpp"
#include "Converter/TileConverter/TilePart.hpp"
#include "Converter/TileConverter/Tile.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Memory.hpp"
#include "Utils/Lz4Lib.hpp"

SM_UNMANAGED_CODE

class BlueprintListReader
{
	BlueprintListReader() = default;

public:
	static void Read(TileReaderContext& context);

	static int Read(
		TileReaderContext& context,
		const std::vector<Byte>& bytes,
		int count);
};

SM_MANAGED_CODE
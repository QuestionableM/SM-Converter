#pragma once

#include "Converter/Entity/Harvestable.hpp"
#include "Converter/ConvertSettings.hpp"
#include "Converter/ConvertError.hpp"

#include "Converter/TileConverter/TileConverterContext.hpp"
#include "Converter/TileConverter/CellHeader.hpp"
#include "Converter/TileConverter/TilePart.hpp"
#include "Converter/TileConverter/Tile.hpp"

#include "ObjectDatabase/UserDataReaders/ItemModCounter.hpp"
#include "ObjectDatabase/Mods/Mod.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Console.hpp"
#include "Utils/Memory.hpp"
#include "Utils/Lz4Lib.hpp"

SM_UNMANAGED_CODE

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
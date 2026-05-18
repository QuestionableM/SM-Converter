#pragma once

#include "Converter/TileConverter/TileConverterContext.hpp"
#include "Converter/TileConverter/CellHeader.hpp"
#include "Converter/TileConverter/Tile.hpp"
#include "Converter/Entity/Asset.hpp"

#include "ObjectDatabase/UserDataReaders/ItemModCounter.hpp"
#include "ObjectDatabase/ObjectDatabase.hpp"
#include "ObjectDatabase/Mods/Mod.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/String.hpp"
#include "Utils/Lz4Lib.hpp"

SM_UNMANAGED_CODE

class AssetListReader
{
	AssetListReader() = default;

public:
	static void Read(TileReaderContext& context);

	static int Read(
		TileReaderContext& context,
		const std::vector<Byte>& bytes,
		int asset_idx,
		int len);
};

SM_MANAGED_CODE
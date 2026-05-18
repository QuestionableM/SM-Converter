#pragma once

#include "HarvestableListReader.hpp"
#include "KinematicsListReader.hpp"
#include "BlueprintListReader.hpp"
#include "AssetListReader.hpp"
#include "DecalListReader.hpp"
#include "ClutterReader.hpp"
#include "PrefabReader.hpp"
#include "MipReader.hpp"

#include "Converter/TileConverter/TileConverterContext.hpp"
#include "Converter/TileConverter/CellHeader.hpp"
#include "Converter/TileConverter/TileHeader.hpp"
#include "Converter/TileConverter/Tile.hpp"

#include "ObjectDatabase/ProgCounter.hpp"
#include "UStd/UnmanagedString.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/String.hpp"
#include "Utils/File.hpp"

SM_UNMANAGED_CODE

class TileReader
{
	TileReader() = default;

public:
	static bool ReadTileHeader(
		const std::wstring& path,
		TileHeaderBaseInfo& out_header,
		ConvertError& error);

	static void ReadTileData(TileReaderContext& context);

	static bool ReadTile(
		const std::wstring_view& path,
		ConvertError& error,
		Tile& outTile,
		const bool counterMode);

	static bool ReadTile(
		const std::vector<Byte>& tileData,
		ConvertError& error,
		Tile& outTile,
		const bool counterMode);
};

SM_MANAGED_CODE
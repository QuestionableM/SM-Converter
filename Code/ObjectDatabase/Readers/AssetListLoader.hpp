#pragma once

#include "UStd\UnmanagedUnorderedMap.hpp"

#include "Utils\Color.hpp"
#include "Utils\Json.hpp"

#pragma unmanaged

class Mod;

class AssetListLoader
{
	AssetListLoader() = default;

	static void LoadDefaultColors(const simdjson::dom::element& jAsset, std::unordered_map<std::string, SMColor>& def_colors);
public:
	static void Load(const simdjson::dom::element& fAssets, Mod* mod);
};

#pragma managed
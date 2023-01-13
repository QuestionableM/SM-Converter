#pragma once

#include "Utils\Color.hpp"
#include "Utils\Json.hpp"

#include <unordered_map>

class Mod;

class AssetListLoader
{
	AssetListLoader() = default;

	static void LoadDefaultColors(const simdjson::dom::element& jAsset, std::unordered_map<std::wstring, SMColor>& def_colors);
public:
	static void Load(const simdjson::dom::element& fAssets, Mod* mod);
};
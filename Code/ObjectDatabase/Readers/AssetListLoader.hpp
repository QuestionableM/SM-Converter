#pragma once

#include "UStd/UnmanagedUnorderedMap.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Hashing.hpp"
#include "Utils/Color.hpp"
#include "Utils/Json.hpp"
#include "Utils/Uuid.hpp"

SM_UNMANAGED_CODE

class SMMod;

class AssetListLoader
{
	AssetListLoader() = default;

	static void LoadDefaultColors(
		const simdjson::dom::element& jAsset,
		std::unordered_map<std::string, SMColor, Hashing::StringHasher, std::equal_to<>>& def_colors);

public:
	static void Load(const simdjson::dom::element& fAssets, SMMod* mod, bool add_to_global_db);
};

SM_MANAGED_CODE
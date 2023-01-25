#pragma once

#include "UStd\UnmanagedUnorderedMap.hpp"

#include "Utils\Color.hpp"
#include "Utils\Json.hpp"
#include "Utils\Uuid.hpp"

#pragma unmanaged

class SMMod;

class AssetListLoader
{
	AssetListLoader() = default;

	static void LoadDefaultColors(const simdjson::dom::element& jAsset, std::unordered_map<std::string, SMColor>& def_colors);
	static bool CheckAssetExists(const SMUuid& v_uuid, SMMod* v_mod, const bool& add_to_global_db);

public:
	static void Load(const simdjson::dom::element& fAssets, SMMod* mod, const bool& add_to_global_db);
};

#pragma managed
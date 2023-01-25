#include "AssetListLoader.hpp"

#include "ObjectDatabase\Readers\DefaultLoader.hpp"
#include "ObjectDatabase\ProgCounter.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"

#pragma unmanaged

void AssetListLoader::LoadDefaultColors(const simdjson::dom::element& jAsset, std::unordered_map<std::string, SMColor>& def_colors)
{
	const auto v_def_colors = jAsset["defaultColors"];
	if (!v_def_colors.is_object()) return;

	for (const auto v_def_col_obj : v_def_colors.get_object())
	{
		if (!v_def_col_obj.value.is_string())
			continue;

		const std::string_view v_str_view = v_def_col_obj.value.get_string();
		const std::string v_color_val = (v_str_view.size() >= 6)
			? std::string(v_str_view.data(), v_str_view.size())
			: "000000";

		const std::string v_key_str(v_def_col_obj.key.data(), v_def_col_obj.key.size());
		if (def_colors.find(v_key_str) != def_colors.end())
			continue;

		def_colors.insert(std::make_pair(v_key_str, v_color_val));
	}
}

bool AssetListLoader::CheckAssetExists(const SMUuid& v_uuid, SMMod* v_mod, const bool& add_to_global_db)
{
	const std::unordered_map<SMUuid, AssetData*>& v_cur_map = add_to_global_db ? SMMod::AssetStorage : v_mod->m_Assets;
	if (v_cur_map.find(v_uuid) == v_cur_map.end())
		return false;

	DebugWarningL("Asset with the same uuid already exists! (", v_uuid.ToString(), ")");
	return true;
}

void AssetListLoader::Load(const simdjson::dom::element& fAssets, SMMod* mod, const bool& add_to_global_db)
{
	if (!fAssets.is_array()) return;

	const auto v_assets_array = fAssets.get_array();
	ProgCounter::ProgressMax += v_assets_array.size();

	for (const auto v_cur_asset : v_assets_array)
	{
		if (!v_cur_asset.is_object()) continue;

		const auto v_uuid = v_cur_asset["uuid"];
		if (!v_uuid.is_string()) continue;

		const SMUuid v_asset_uuid = v_uuid.get_c_str();
		if (AssetListLoader::CheckAssetExists(v_asset_uuid, mod, add_to_global_db))
			continue;

		std::wstring v_tMesh;
		SMSubMeshBase* v_tData;
		if (!DefaultLoader::LoadRenderable(v_cur_asset, &v_tData, v_tMesh))
			continue;

		AssetData* v_new_asset = new AssetData();
		v_new_asset->m_uuid = v_asset_uuid;
		v_new_asset->m_mesh = v_tMesh;
		AssetListLoader::LoadDefaultColors(v_cur_asset, v_new_asset->m_defaultColors);
		v_new_asset->m_textures = v_tData;
		v_new_asset->m_mod = mod;

		const auto v_new_pair = std::make_pair(v_new_asset->m_uuid, v_new_asset);

		mod->m_Assets.insert(v_new_pair);

		if (add_to_global_db)
			SMMod::AssetStorage.insert(v_new_pair);

		ProgCounter::ProgressValue++;
	}
}
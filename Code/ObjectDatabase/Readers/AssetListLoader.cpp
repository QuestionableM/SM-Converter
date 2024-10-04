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

void AssetListLoader::Load(const simdjson::dom::element& fAssets, SMMod* mod, bool add_to_global_db)
{
	if (!fAssets.is_array()) return;

	const auto v_assets_array = fAssets.get_array();
	ProgCounter::ProgressMax += v_assets_array.size();

	auto& v_cur_db = mod->m_Assets.GetStorage(add_to_global_db);
	auto v_adder_func = mod->m_Assets.GetAdderFunction(add_to_global_db);

	for (const auto v_cur_asset : v_assets_array)
	{
		if (!v_cur_asset.is_object()) continue;

		const auto v_uuid = v_cur_asset["uuid"];
		if (!v_uuid.is_string()) continue;

		const SMUuid v_assetUuid = v_uuid.get_c_str().value();
		if (mod->m_Assets.ObjectExists(v_cur_db, v_assetUuid))
			continue;

		std::wstring v_meshPath;
		SMSubMeshBase* v_subMeshData;
		if (!DefaultLoader::LoadRenderable(v_cur_asset, &v_subMeshData, v_meshPath))
			continue;

		AssetData* v_pNewAsset = new AssetData(v_assetUuid, std::move(v_meshPath), v_subMeshData, mod);
		AssetListLoader::LoadDefaultColors(v_cur_asset, v_pNewAsset->m_defaultColors);

		(mod->m_Assets.*v_adder_func)(v_pNewAsset);
		ProgCounter::ProgressValue++;
	}
}
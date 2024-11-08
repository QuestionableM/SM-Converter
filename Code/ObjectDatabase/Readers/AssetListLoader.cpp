#include "AssetListLoader.hpp"

#include "ObjectDatabase\Readers\DefaultLoader.hpp"
#include "ObjectDatabase\ProgCounter.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"

#pragma unmanaged

void AssetListLoader::LoadDefaultColors(
	const simdjson::dom::element& jAsset,
	std::unordered_map<std::string, SMColor, Hashing::StringHasher, std::equal_to<>>& def_colors)
{
	const auto v_def_colors = jAsset["defaultColors"];
	if (!v_def_colors.is_object()) return;

	for (const auto v_def_col_obj : v_def_colors.get_object().value_unsafe())
	{
		if (!v_def_col_obj.value.is_string())
			continue;

		const std::string_view v_strView = v_def_col_obj.value.get_string().value_unsafe();
		const std::string_view v_colorVal = (v_strView.size() >= 6) ? v_strView : "000000";

		if (def_colors.find(v_def_col_obj.key) != def_colors.end())
			continue;

		def_colors.emplace(v_def_col_obj.key, v_colorVal);
	}
}

void AssetListLoader::Load(const simdjson::dom::element& fAssets, SMMod* mod, bool add_to_global_db)
{
	if (!fAssets.is_array()) return;

	const auto v_assets_array = fAssets.get_array().value_unsafe();
	ProgCounter::ProgressMax += v_assets_array.size();

	auto& v_cur_db = mod->m_Assets.GetStorage(add_to_global_db);
	auto v_adder_func = mod->m_Assets.GetAdderFunction(add_to_global_db);

	std::wstring v_meshPath;

	for (const auto v_cur_asset : v_assets_array)
	{
		if (!v_cur_asset.is_object()) continue;

		const auto v_uuid = v_cur_asset["uuid"];
		if (!v_uuid.is_string()) continue;

		const SMUuid v_assetUuid = v_uuid.get_string().value_unsafe();
		if (mod->m_Assets.ObjectExists(v_cur_db, v_assetUuid))
			continue;

		SMSubMeshBase* v_subMeshData;
		if (!DefaultLoader::LoadRenderable(v_cur_asset, &v_subMeshData, v_meshPath))
			continue;

		AssetData* v_pNewAsset = new AssetData(v_assetUuid, std::move(v_meshPath), v_subMeshData, mod);
		AssetListLoader::LoadDefaultColors(v_cur_asset, v_pNewAsset->m_defaultColors);

		(mod->m_Assets.*v_adder_func)(v_pNewAsset);
		ProgCounter::ProgressValue++;
	}
}
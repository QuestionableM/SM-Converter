#include "HarvestableListLoader.hpp"

#include "ObjectDatabase\Readers\DefaultLoader.hpp"
#include "ObjectDatabase\ProgCounter.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\Console.hpp"

#pragma unmanaged

bool HarvestableListLoader::CheckHarvestableExists(const SMUuid& v_uuid, SMMod* v_mod, const bool& add_to_global_db)
{
	const std::unordered_map<SMUuid, HarvestableData*>& v_cur_list = add_to_global_db ? SMMod::HarvestableStorage : v_mod->m_Harvestables;
	if (v_cur_list.find(v_uuid) == v_cur_list.end())
		return false;

	DebugWarningL("Harvestable with the specified uuid already exists! (", v_uuid.ToString(), ")");
	return true;
}

void HarvestableListLoader::Load(const simdjson::dom::element& fHarvestables, SMMod* mod, const bool& add_to_global_db)
{
	if (!fHarvestables.is_array()) return;
	
	const auto v_hvs_array = fHarvestables.get_array();
	ProgCounter::ProgressMax += v_hvs_array.size();

	for (const auto v_hvs : v_hvs_array)
	{
		if (!v_hvs.is_object()) continue;

		const auto v_uuid_obj = v_hvs["uuid"];
		if (!v_uuid_obj.is_string()) continue;

		const SMUuid v_hvs_uuid = v_uuid_obj.get_c_str();
		if (HarvestableListLoader::CheckHarvestableExists(v_hvs_uuid, mod, add_to_global_db))
			continue;

		std::wstring v_tMesh;
		SMSubMeshBase* v_tData;
		if (!DefaultLoader::LoadRenderable(v_hvs, &v_tData, v_tMesh))
			continue;

		HarvestableData* v_new_hvs = new HarvestableData();
		v_new_hvs->m_uuid = v_hvs_uuid;
		v_new_hvs->m_mesh = v_tMesh;
		v_new_hvs->m_textures = v_tData;
		v_new_hvs->m_mod = mod;

		const auto v_new_pair = std::make_pair(v_new_hvs->m_uuid, v_new_hvs);

		mod->m_Harvestables.insert(v_new_pair);

		if (add_to_global_db)
			SMMod::HarvestableStorage.insert(v_new_pair);

		ProgCounter::ProgressValue++;
	}
}
#include "HarvestableListLoader.hpp"

#include "ObjectDatabase\Readers\DefaultLoader.hpp"
#include "ObjectDatabase\ProgCounter.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\Console.hpp"

#pragma unmanaged

void HarvestableListLoader::Load(const simdjson::dom::element& fHarvestables, SMMod* mod, bool add_to_global_db)
{
	if (!fHarvestables.is_array()) return;
	
	const auto v_hvs_array = fHarvestables.get_array().value_unsafe();
	ProgCounter::ProgressMax += v_hvs_array.size();

	auto& v_cur_db = mod->m_Harvestables.GetStorage(add_to_global_db);
	auto v_adder_func = mod->m_Harvestables.GetAdderFunction(add_to_global_db);

	for (const auto v_hvs : v_hvs_array)
	{
		if (!v_hvs.is_object()) continue;

		const auto v_uuid_obj = v_hvs["uuid"];
		if (!v_uuid_obj.is_string()) continue;

		const SMUuid v_hvs_uuid = v_uuid_obj.get_string().value_unsafe();
		if (mod->m_Harvestables.ObjectExists(v_cur_db, v_hvs_uuid))
			continue;

		std::wstring v_tMesh;
		SMSubMeshBase* v_tData;
		if (!DefaultLoader::LoadRenderable(v_hvs, &v_tData, v_tMesh))
			continue;

		HarvestableData* v_new_hvs = new HarvestableData(
			v_hvs_uuid,
			std::move(v_tMesh),
			v_tData,
			mod
		);

		(mod->m_Harvestables.*v_adder_func)(v_new_hvs);
		ProgCounter::ProgressValue++;
	}
}
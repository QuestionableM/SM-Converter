#include "KinematicListLoader.hpp"

#include "ObjectDatabase\Readers\DefaultLoader.hpp"
#include "ObjectDatabase\ProgCounter.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

SM_UNMANAGED_CODE

void KinematicListLoader::Load(const simdjson::dom::element& v_kinematics, SMMod* mod, bool add_to_global_db)
{
	if (!v_kinematics.is_array()) return;

	const auto v_kinematics_array = v_kinematics.get_array().value_unsafe();
	ProgCounter::ProgressMax += v_kinematics_array.size();

	auto& v_cur_db = mod->m_Kinematics.GetStorage(add_to_global_db);
	auto v_adder_func = mod->m_Kinematics.GetAdderFunction(add_to_global_db);

	std::wstring v_kmMesh;

	for (const auto v_kinematic : v_kinematics.get_array().value_unsafe())
	{
		if (!v_kinematic.is_object()) continue;

		const auto v_uuid = v_kinematic["uuid"];
		if (!v_uuid.is_string()) continue;

		const SMUuid v_km_uuid = v_uuid.get_string().value_unsafe();
		if (mod->m_Kinematics.ObjectExists(v_cur_db, v_km_uuid))
			continue;

		SMSubMeshBase* v_KmSubMeshes;
		if (!DefaultLoader::LoadRenderable(v_kinematic, &v_KmSubMeshes, v_kmMesh))
			continue;

		KinematicData* v_new_kinematic = new KinematicData(
			v_km_uuid,
			std::move(v_kmMesh),
			v_KmSubMeshes,
			mod
		);

		(mod->m_Kinematics.*v_adder_func)(v_new_kinematic);
		ProgCounter::ProgressValue++;
	}
}
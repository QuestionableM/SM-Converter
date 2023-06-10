#include "KinematicListLoader.hpp"

#include "ObjectDatabase\Readers\DefaultLoader.hpp"
#include "ObjectDatabase\ProgCounter.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#pragma unmanaged

void KinematicListLoader::Load(const simdjson::dom::element& v_kinematics, SMMod* mod, bool add_to_global_db)
{
	if (!v_kinematics.is_array()) return;

	const auto v_kinematics_array = v_kinematics.get_array();
	ProgCounter::ProgressMax += v_kinematics_array.size();

	auto& v_cur_db = mod->m_Kinematics.GetStorage(add_to_global_db);
	auto v_adder_func = mod->m_Kinematics.GetAdderFunction(add_to_global_db);

	for (const auto v_kinematic : v_kinematics.get_array())
	{
		if (!v_kinematic.is_object()) continue;

		const auto v_uuid = v_kinematic["uuid"];
		if (!v_uuid.is_string()) continue;

		const SMUuid v_km_uuid = v_uuid.get_c_str();
		if (mod->m_Kinematics.ObjectExists(v_cur_db, v_km_uuid))
			continue;

		std::wstring v_km_mesh;
		SMSubMeshBase* v_km_subMeshes;
		if (!DefaultLoader::LoadRenderable(v_kinematic, &v_km_subMeshes, v_km_mesh))
			continue;

		KinematicData* v_new_kinematic = new KinematicData();
		v_new_kinematic->m_uuid = v_km_uuid;
		v_new_kinematic->m_mesh = std::move(v_km_mesh);
		v_new_kinematic->m_textures = v_km_subMeshes;
		v_new_kinematic->m_mod = mod;

		(mod->m_Kinematics.*v_adder_func)(v_new_kinematic);
		ProgCounter::ProgressValue++;
	}
}
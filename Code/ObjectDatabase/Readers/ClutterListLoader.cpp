#include "ClutterListLoader.hpp"

#include "ObjectDatabase/KeywordReplacer.hpp"
#include "ObjectDatabase/ProgCounter.hpp"
#include "ObjectDatabase/Mods/Mod.hpp"

#include "Utils/Console.hpp"
#include "Utils/String.hpp"

SM_UNMANAGED_CODE

bool ClutterListLoader::LoadTextureData(const simdjson::dom::element& fClutter, SMTextureList& tList, std::wstring& mesh)
{
	const auto v_cl_dif = fClutter["texture"];
	if (!v_cl_dif.is_string()) return false;

	const auto v_cl_mesh = fClutter["mesh"];
	if (!v_cl_mesh.is_string()) return false;

	String::ToWideRef(v_cl_dif.get_string().value_unsafe(), tList.m_dif);
	KeywordReplacer::ReplaceKeyR(tList.m_dif);

	const auto v_cl_material = fClutter["material"];
	tList.m_material.assign(v_cl_material.is_string() ? v_cl_material.get_string().value_unsafe() : "GroundVegetation");

	String::ToWideRef(v_cl_mesh.get_string().value_unsafe(), mesh);
	KeywordReplacer::ReplaceKeyR(mesh);

	return true;
}

ClutterDataParams ClutterListLoader::LoadClutterData(const simdjson::dom::element& fClutter)
{
	const auto v_cl_height = fClutter["height"];
	const auto v_cl_scale_variance = fClutter["scaleVariance"];
	const auto v_cl_ground_normal = fClutter["groundNormal"];

	ClutterDataParams v_output;
	v_output.m_height = (v_cl_height.is_number() ? JsonReader::GetNumber<float>(v_cl_height.value_unsafe()) : 1.0f);
	v_output.m_scaleVariance = (v_cl_scale_variance.is_number() ? JsonReader::GetNumber<float>(v_cl_scale_variance.value_unsafe()) : 0.0f);
	v_output.m_groundNormal = (v_cl_ground_normal.is_bool() ? v_cl_ground_normal.get_bool().value_unsafe() : false);

	return v_output;
}

void ClutterListLoader::Load(const simdjson::dom::element& fClutter, SMMod* mod, bool add_to_global_db)
{
	if (!fClutter.is_array()) return;

	const auto v_clutter_array = fClutter.get_array().value_unsafe();
	ProgCounter::ProgressMax += v_clutter_array.size();

	SMTextureList v_clutterTextures;
	std::wstring v_clutterMesh;

	for (const auto v_clutter : v_clutter_array)
	{
		if (!v_clutter.is_object()) continue;

		const auto v_uuid = v_clutter["uuid"];
		if (!v_uuid.is_string()) continue;

		const SMUuid v_clutter_uuid = v_uuid.get_string().value_unsafe();
		if (mod->m_Clutter.ObjectExists(v_clutter_uuid, false))
		{
			DebugErrorL("Clutter with the specified uuid already exists! (", v_clutter_uuid.toString(), ")");
			continue;
		}

		if (!ClutterListLoader::LoadTextureData(v_clutter, v_clutterTextures, v_clutterMesh))
		{
			DebugErrorL("Couldn't load the texture data for: ", v_clutter_uuid.toString());
			continue;
		}

		ClutterData* v_new_clutter = new ClutterData(
			v_clutter_uuid,
			std::move(v_clutterMesh),
			std::move(v_clutterTextures),
			ClutterListLoader::LoadClutterData(v_clutter),
			mod
		);

		mod->m_Clutter.AddObject(v_new_clutter, true);
		SMMod::ClutterVector.push_back(v_new_clutter);

		ProgCounter::ProgressValue++;
	}
}
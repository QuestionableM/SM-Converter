#include "ClutterListLoader.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"
#include "ObjectDatabase\ProgCounter.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"

#pragma unmanaged

bool ClutterListLoader::LoadTextureData(const simdjson::dom::element& fClutter, SMTextureList& tList, std::wstring& mesh)
{
	const auto v_cl_dif = fClutter["texture"];
	if (!v_cl_dif.is_string()) return false;

	const auto v_cl_mesh = fClutter["mesh"];
	if (!v_cl_mesh.is_string()) return false;

	const std::wstring v_cl_dif_wstr = String::ToWide(v_cl_dif.get_string());
	tList.dif = KeywordReplacer::ReplaceKey(v_cl_dif_wstr);

	const auto v_cl_material = fClutter["material"];
	tList.material = (v_cl_material.is_string() ? v_cl_material.get_c_str() : "GroundVegetation");

	const std::wstring v_cl_mesh_wstr = String::ToWide(v_cl_mesh.get_string());
	mesh = KeywordReplacer::ReplaceKey(v_cl_mesh_wstr);

	return true;
}

void ClutterListLoader::LoadClutterData(const simdjson::dom::element& fClutter, ClutterData* pClutter)
{
	const auto v_cl_height = fClutter["height"];
	const auto v_cl_scale_variance = fClutter["scaleVariance"];
	const auto v_cl_ground_normal = fClutter["groundNormal"];

	pClutter->m_height = (v_cl_height.is_number() ? JsonReader::GetNumber<float>(v_cl_height) : 1.0f);
	pClutter->m_scaleVariance = (v_cl_scale_variance.is_number() ? JsonReader::GetNumber<float>(v_cl_scale_variance) : 0.0f);
	pClutter->m_groundNormal = (v_cl_ground_normal.is_bool() ? v_cl_ground_normal.get_bool() : false);
}

void ClutterListLoader::Load(const simdjson::dom::element& fClutter, SMMod* mod)
{
	if (!fClutter.is_array()) return;

	const auto v_clutter_array = fClutter.get_array();
	ProgCounter::ProgressMax += v_clutter_array.size();

	for (const auto v_clutter : v_clutter_array)
	{
		if (!v_clutter.is_object()) continue;

		const auto v_uuid = v_clutter["uuid"];
		if (!v_uuid.is_string()) continue;

		const SMUuid v_clutter_uuid = v_uuid.get_c_str();
		if (SMMod::ClutterStorage.find(v_clutter_uuid) != SMMod::ClutterStorage.end())
		{
			DebugErrorL("Clutter with the specified uuid already exists! (", v_clutter_uuid.ToString(), ")");
			continue;
		}

		SMTextureList v_tList;
		std::wstring v_tMesh;
		if (!ClutterListLoader::LoadTextureData(v_clutter, v_tList, v_tMesh))
		{
			DebugErrorL("Couldn't load the texture data for: ", v_clutter_uuid.ToString());
			continue;
		}

		ClutterData* v_new_clutter = new ClutterData();
		v_new_clutter->m_uuid = v_clutter_uuid;
		v_new_clutter->m_mesh = v_tMesh;
		v_new_clutter->m_textures = v_tList;
		v_new_clutter->m_mod = mod;

		ClutterListLoader::LoadClutterData(v_clutter, v_new_clutter);

		const auto v_new_pair = std::make_pair(v_new_clutter->m_uuid, v_new_clutter);

		mod->m_Clutter.insert(v_new_pair);
		SMMod::ClutterStorage.insert(v_new_pair);
		SMMod::ClutterVector.push_back(v_new_clutter);

		ProgCounter::ProgressValue++;
	}
}
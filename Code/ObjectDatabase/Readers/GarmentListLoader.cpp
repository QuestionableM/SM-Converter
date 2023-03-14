#include "GarmentListLoader.hpp"

#include "ObjectDatabase\Readers\DefaultLoader.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#pragma unmanaged

void GarmentListLoader::Load(const simdjson::dom::element& v_garments, SMMod* mod, const bool& add_to_global_db)
{
	if (!v_garments.is_array())
		return;

	for (const auto& v_garment_category : v_garments.get_array())
		GarmentListLoader::LoadGarmentCategory(v_garment_category);
}

void GarmentListLoader::LoadGarmentCategory(const simdjson::dom::element& v_category)
{
	if (!v_category.is_object())
		return;

	const auto v_category_name_obj = v_category["name"];
	const auto v_category_options = v_category["options"];

	if (!(v_category_name_obj.is_string() && v_category_options.is_array()))
		return;

	const std::string v_category_name = v_category_name_obj.get_c_str();
	if (SMMod::GarmentStorage.find(v_category_name) != SMMod::GarmentStorage.end())
	{
		DebugWarningL("The specified garment category already exists: ", v_category_name);
		return;
	}

	std::unordered_map<SMUuid, GarmentData*> v_category_data = {};

	for (const auto& v_garment : v_category_options.get_array())
	{
		if (!v_garment.is_object()) continue;

		const auto v_garment_uuid_obj = v_garment["uuid"];
		if (!v_garment_uuid_obj.is_string())
			continue;

		const auto v_male_rend_path = v_garment["male"];
		const auto v_female_rend_path = v_garment["female"];
		if (!(v_male_rend_path.is_string() && v_female_rend_path.is_string()))
			continue;

		std::wstring v_male_obj;
		SMSubMeshBase* v_male_rend;
		if (!DefaultLoader::LoadRenderableFromPath(v_male_rend_path.value_unsafe(), &v_male_rend, v_male_obj))
			continue;

		std::wstring v_female_obj;
		SMSubMeshBase* v_female_rend;
		if (!DefaultLoader::LoadRenderableFromPath(v_female_rend_path.value_unsafe(), &v_female_rend, v_female_obj))
		{
			delete v_male_rend;
			continue;
		}

		const SMUuid v_garment_uuid = v_garment_uuid_obj.get_c_str();
		if (v_category_data.find(v_garment_uuid) != v_category_data.end())
		{
			DebugWarningL("The specified garment already exists (Uuid: ", v_garment_uuid.ToString(), ")");
			continue;
		}

		GarmentData* v_new_garment = new GarmentData();
		v_new_garment->m_female_data = v_female_rend;
		v_new_garment->m_female_mesh = std::move(v_female_obj);
		v_new_garment->m_male_data = v_male_rend;
		v_new_garment->m_male_mesh = std::move(v_male_obj);
		v_new_garment->m_uuid = v_garment_uuid;

		v_category_data.emplace(v_new_garment->m_uuid, v_new_garment);
	}

	SMMod::GarmentStorage.emplace(v_category_name, std::move(v_category_data));
}
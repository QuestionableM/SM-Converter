#include "GarmentListLoader.hpp"

#include "ObjectDatabase\Readers\DefaultLoader.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

SM_UNMANAGED_CODE

void GarmentListLoader::Load(const simdjson::dom::element& v_garments, SMMod* mod, bool add_to_global_db)
{
	if (!v_garments.is_array()) return;

	for (const auto& v_garment_category : v_garments.get_array().value_unsafe())
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

	const std::string_view v_categoryName(v_category_name_obj.get_string().value_unsafe());
	if (SMMod::GarmentStorage.find(v_categoryName) != SMMod::GarmentStorage.end())
	{
		DebugWarningL("The specified garment category already exists: ", v_categoryName);
		return;
	}

	std::unordered_map<SMUuid, GarmentData*> v_categoryData;
	std::wstring v_maleObj;
	std::wstring v_femaleObj;

	for (const auto& v_garment : v_category_options.get_array().value_unsafe())
	{
		if (!v_garment.is_object()) continue;

		const auto v_garmentUuidObj = v_garment["uuid"];
		if (!v_garmentUuidObj.is_string())
			continue;

		const auto v_maleRendPath = v_garment["male"];
		const auto v_femaleRendPath = v_garment["female"];
		if (!(v_maleRendPath.is_string() && v_femaleRendPath.is_string()))
			continue;

		SMSubMeshBase* v_maleRend;
		if (!DefaultLoader::LoadRenderableFromPath(v_maleRendPath.get_string().value_unsafe(), &v_maleRend, v_maleObj))
			continue;

		SMSubMeshBase* v_femaleRend;
		if (!DefaultLoader::LoadRenderableFromPath(v_femaleRendPath.get_string().value_unsafe(), &v_femaleRend, v_femaleObj))
		{
			delete v_maleRend;
			continue;
		}

		const SMUuid v_garmentUuid = v_garmentUuidObj.get_string().value_unsafe();
		if (v_categoryData.find(v_garmentUuid) != v_categoryData.end())
		{
			DebugWarningL("The specified garment already exists (Uuid: ", v_garmentUuid.toString(), ")");
			continue;
		}

		GarmentData* v_newGarment = new GarmentData(
			v_garmentUuid,
			std::move(v_maleObj),
			std::move(v_femaleObj),
			v_maleRend,
			v_femaleRend
		);

		v_categoryData.emplace(v_newGarment->m_uuid, v_newGarment);
	}

	SMMod::GarmentStorage.emplace(v_categoryName, std::move(v_categoryData));
}
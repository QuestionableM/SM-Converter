#include "DecalsetReader.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\Json.hpp"

#pragma unmanaged

inline void GetWstringFromDecalset(const simdjson::dom::element& obj, const std::string& key, std::wstring& r_output)
{
	const auto v_json_str = obj[key];
	if (v_json_str.is_string())
	{
		String::ToWideRef(v_json_str.get_string().value_unsafe(), r_output);
		KeywordReplacer::ReplaceKeyR(r_output);
	}
}

void DecalsetReader::LoadFromFile(const std::wstring& path, SMMod* mod, bool add_to_global_db)
{
	simdjson::dom::document v_json_doc;
	if (!JsonReader::LoadParseSimdjsonCommentsC(path, v_json_doc, simdjson::dom::element_type::OBJECT))
		return;

	const auto v_root = v_json_doc.root();

	const auto v_decal_list = v_root["decalList"];
	if (!v_decal_list.is_object())
		return;

	SMTextureList v_texList;
	GetWstringFromDecalset(v_root, "difSheet", v_texList.m_dif);
	GetWstringFromDecalset(v_root, "asgSheet", v_texList.m_asg);
	GetWstringFromDecalset(v_root, "norSheet", v_texList.m_nor);

	for (const auto v_decal : v_decal_list.get_object().value_unsafe())
	{
		if (!v_decal.value.is_object()) continue;

		const auto v_uuid = v_decal.value["uuid"];
		const auto v_material = v_decal.value["material"];

		if (!(v_uuid.is_string() && v_material.is_string()))
			continue;

		const auto v_decal_region = v_decal.value["region"];
		if (!v_decal_region.is_array())
			continue;

		const auto v_decal_reg_array = v_decal_region.get_array().value_unsafe();
		if (v_decal_reg_array.size() != 4)
			continue;

		const SMUuid v_decal_uuid = v_uuid.get_string().value_unsafe();
		if (mod->m_Decals.ObjectExists(v_decal_uuid, add_to_global_db))
			continue;

		v_texList.m_material.assign(v_material.get_string().value_unsafe());

		DecalData* v_new_decal = new DecalData(
			v_decal_uuid,
			v_decal.key,
			v_texList,
			mod
		);

		if (v_decal_reg_array.size() >= 4)
		{
			for (std::size_t a = 0; a < 4; a++)
				v_new_decal->m_ranges[a] = JsonReader::GetNumber<int>(v_decal_reg_array.at(a).value_unsafe());
		}

		mod->m_Decals.AddObject(v_new_decal, add_to_global_db);
	}
}
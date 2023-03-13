#include "DefaultLoader.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"

#pragma unmanaged

void DefaultLoader::LoadTextureList(const simdjson::dom::array& texList, SMTextureList* v_tex_list)
{
	const int v_arr_sz = static_cast<int>(texList.size());
	const int v_list_sz = (v_arr_sz > 3 ? 3 : v_arr_sz);

	for (int a = 0; a < v_list_sz; a++)
	{
		const auto v_cur_item = texList.at(a);

		if (v_cur_item.is_string())
		{
			std::wstring& v_wstr_path = v_tex_list->GetStringRef(a);

			v_wstr_path = String::ToWide(v_cur_item.get_string());
			KeywordReplacer::ReplaceKeyR(v_wstr_path);
		}
	}
}

bool DefaultLoader::LoadSubMeshDataEntry(const simdjson::dom::element& v_item, SMTextureList** v_sub_mesh)
{
	const auto v_material = v_item["material"];
	const auto v_tex_list_obj = v_item["textureList"];

	if (!(v_material.is_string() && v_tex_list_obj.is_array()))
		return false;

	SMTextureList* v_newEntry = new SMTextureList();

	const std::string_view v_mat_view = v_material.get_string();
	v_newEntry->material = std::string(v_mat_view.data(), v_mat_view.size());

	const auto v_custom_prop = v_item["custom"];
	if (v_custom_prop.is_object())
	{
		const auto v_def_col_idx = v_custom_prop["color"];
		if (v_def_col_idx.is_string())
		{
			const std::string_view v_col_view = v_def_col_idx.get_string();
			v_newEntry->def_color_idx = std::string(v_col_view.data(), v_col_view.size());
		}

		const auto v_shadow_only_mode = v_custom_prop["shadowOnly"];
		if (v_shadow_only_mode.is_bool())
			v_newEntry->is_shadow_only = v_shadow_only_mode.get_bool();
	}


	DefaultLoader::LoadTextureList(v_tex_list_obj.value_unsafe(), v_newEntry);

	(*v_sub_mesh) = v_newEntry;
	return true;
}

bool DefaultLoader::TryLoadSubMeshList(const simdjson::dom::element& lod_item, SMSubMeshBase** v_sub_mesh)
{
	const auto v_sub_mesh_list_obj = lod_item["subMeshList"];
	if (!v_sub_mesh_list_obj.is_array())
		return false;

	std::size_t v_idx = 0;
	SMSubMeshList* v_subMeshList = new SMSubMeshList();

	for (const auto v_sub_mesh_item : v_sub_mesh_list_obj.get_array())
	{
		if (!v_sub_mesh_item.is_object()) continue;

		const auto v_idx_json = v_sub_mesh_item["idx"];
		const std::size_t v_cur_idx = (v_idx_json.is_number() ? JsonReader::GetNumber<std::size_t>(v_idx_json) : v_idx);

		SMTextureList* v_newTexData;
		if (DefaultLoader::LoadSubMeshDataEntry(v_sub_mesh_item, &v_newTexData))
			v_subMeshList->AddTexList("", v_cur_idx, v_newTexData);

		v_idx++;
	}

	(*v_sub_mesh) = v_subMeshList;
	return true;
}

bool DefaultLoader::TryLoadSubMeshMap(const simdjson::dom::element& lod_item, SMSubMeshBase** v_sub_mesh)
{
	const auto v_sub_mesh_map_obj = lod_item["subMeshMap"];
	if (!v_sub_mesh_map_obj.is_object())
		return false;

	SMSubMeshMap* v_subMeshMap = new SMSubMeshMap();

	for (const auto v_sub_mesh_item : v_sub_mesh_map_obj.get_object())
	{
		if (!v_sub_mesh_item.value.is_object()) continue;

		const std::string v_sub_mesh_name(v_sub_mesh_item.key.data(), v_sub_mesh_item.key.size());

		SMTextureList* v_newTexData;
		if (DefaultLoader::LoadSubMeshDataEntry(v_sub_mesh_item.value, &v_newTexData))
			v_subMeshMap->AddTexList(v_sub_mesh_name, 0, v_newTexData);
	}

	(*v_sub_mesh) = v_subMeshMap;
	return true;
}

bool DefaultLoader::LoadRenderableData(const simdjson::dom::element& jRenderable, SMSubMeshBase** tData, std::wstring& mesh)
{
	const auto v_lod_list_obj = jRenderable["lodList"];
	if (!v_lod_list_obj.is_array())
		return false;

	const auto v_lod_list_array = v_lod_list_obj.get_array();
	if (v_lod_list_array.size() == 0)
		return false;

	const auto v_lod_list0 = v_lod_list_array.at(0);
	if (!v_lod_list0.is_object())
		return false;

	if (!DefaultLoader::TryLoadSubMeshList(v_lod_list0.value_unsafe(), tData))
	{
		if (!DefaultLoader::TryLoadSubMeshMap(v_lod_list0.value_unsafe(), tData))
			return false;
	}

	const auto v_mesh_path = v_lod_list0["mesh"];
	if (!v_mesh_path.is_string())
		return false;

	mesh = String::ToWide(v_mesh_path.get_string());
	KeywordReplacer::ReplaceKeyR(mesh);

	return true;
}

bool DefaultLoader::LoadRenderableFromPath(const simdjson::dom::element& v_path, SMSubMeshBase** tData, std::wstring& mesh)
{
	std::wstring v_renderablePath = String::ToWide(v_path.get_string());
	KeywordReplacer::ReplaceKeyR(v_renderablePath);

	simdjson::dom::document v_rend_doc;
	if (!JsonReader::LoadParseSimdjsonCommentsC(v_renderablePath, v_rend_doc, simdjson::dom::element_type::OBJECT))
		return false;

	return DefaultLoader::LoadRenderableData(v_rend_doc.root(), tData, mesh);
}

bool DefaultLoader::LoadRenderable(const simdjson::dom::element& jAsset, SMSubMeshBase** tData, std::wstring& mesh)
{
	const auto v_rend_data = jAsset["renderable"];
	if (v_rend_data.error() != simdjson::SUCCESS)
		return false;

	switch (v_rend_data.type())
	{
	case simdjson::dom::element_type::STRING:
		return DefaultLoader::LoadRenderableFromPath(v_rend_data.value_unsafe(), tData, mesh);
	case simdjson::dom::element_type::OBJECT:
		return DefaultLoader::LoadRenderableData(v_rend_data.value_unsafe(), tData, mesh);
	default:
		return false;
	}
}
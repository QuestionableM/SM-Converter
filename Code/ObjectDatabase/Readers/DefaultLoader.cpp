#include "DefaultLoader.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"
#include "Utils\String.hpp"

TextureList DefaultLoader::LoadTextureList(const simdjson::dom::array& texList)
{
	TextureList new_list;

	const int v_arr_sz = static_cast<int>(texList.size());
	const int v_list_sz = (v_arr_sz > 3 ? 3 : v_arr_sz);

	for (int a = 0; a < v_list_sz; a++)
	{
		const auto v_cur_item = texList.at(a);

		if (v_cur_item.is_string())
		{
			std::wstring v_wstr_path = new_list.GetStringRef(a);

			v_wstr_path = String::ToWide(v_cur_item.get_string());
			KeywordReplacer::ReplaceKeyR(v_wstr_path);
		}
	}

	return new_list;
}

void DefaultLoader::AddSubMesh(const simdjson::dom::element& subMesh, TextureData& tData, const std::wstring& idx)
{
	const auto v_tex_list = subMesh["textureList"];
	if (!v_tex_list.is_array()) return;

	TextureList v_new_tList = DefaultLoader::LoadTextureList(v_tex_list.get_array());

	const auto v_custom_prop = subMesh["custom"];
	if (v_custom_prop.is_object())
	{
		const auto v_def_col_idx = v_custom_prop["color"];
		if (v_def_col_idx.is_string())
			v_new_tList.def_color_idx = String::ToWide(v_def_col_idx.get_string());
	}

	const auto v_mat_name = subMesh["material"];
	if (v_mat_name.is_string())
		v_new_tList.material = String::ToWide(v_mat_name.get_string());

	tData.AddEntry(idx, v_new_tList);
}

bool DefaultLoader::LoadTextureData(const simdjson::dom::element& jLodList, TextureData& tData)
{
	const auto v_sub_mesh_list_obj = jLodList["subMeshList"];
	if (v_sub_mesh_list_obj.is_array())
	{
		std::size_t _idx = 0;
		tData = TextureData(TextureDataType::SubMeshList);

		for (const auto v_sub_mesh_item : v_sub_mesh_list_obj.get_array())
		{
			if (!v_sub_mesh_item.is_object()) continue;

			const auto v_idx = v_sub_mesh_item["idx"];
			std::size_t v_cur_idx = (v_idx.is_number() ? JsonReader::GetNumber<std::size_t>(v_idx) : _idx);

			DefaultLoader::AddSubMesh(v_sub_mesh_item, tData, std::to_wstring(v_cur_idx));
			_idx++;
		}

		return true;
	}

	const auto v_sub_mesh_map_obj = jLodList["subMeshMap"];
	if (v_sub_mesh_map_obj.is_object())
	{
		tData = TextureData(TextureDataType::SubMeshMap);

		for (const auto v_sub_mesh_item : v_sub_mesh_map_obj.get_object())
		{
			if (!v_sub_mesh_item.value.is_object()) continue;

			const std::wstring v_sub_mesh_name = String::ToWide(v_sub_mesh_item.key);
			DefaultLoader::AddSubMesh(v_sub_mesh_item.value, tData, v_sub_mesh_name);
		}

		return true;
	}

	return false;
}

bool DefaultLoader::LoadRenderableData(const simdjson::dom::element& jRenderable, TextureData& tData, std::wstring& mesh)
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

	if (!DefaultLoader::LoadTextureData(v_lod_list0.value_unsafe(), tData))
		return false;

	const auto v_mesh_path = v_lod_list0["mesh"];
	if (!v_mesh_path.is_string())
		return false;

	mesh = String::ToWide(v_mesh_path.get_string());
	KeywordReplacer::ReplaceKeyR(mesh);

	return true;
}

bool DefaultLoader::LoadRenderable(const simdjson::dom::element& jAsset, TextureData& tData, std::wstring& mesh)
{
	const auto v_rend_data = jAsset["renderable"];
	if (v_rend_data.error() != simdjson::SUCCESS)
		return false;

	switch (v_rend_data.type())
	{
	case simdjson::dom::element_type::STRING:
		{
			std::wstring v_renderablePath = String::ToWide(v_rend_data.get_string());
			KeywordReplacer::ReplaceKeyR(v_renderablePath);

			simdjson::dom::document v_rend_doc;
			if (!JsonReader::LoadParseSimdjsonCommentsC(v_renderablePath, v_rend_doc, simdjson::dom::element_type::OBJECT))
				return false;

			return DefaultLoader::LoadRenderableData(v_rend_doc.root(), tData, mesh);
		}
	case simdjson::dom::element_type::OBJECT:
		return DefaultLoader::LoadRenderableData(v_rend_data.value_unsafe(), tData, mesh);
	default:
		return false;
	}
}
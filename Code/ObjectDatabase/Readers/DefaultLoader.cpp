#include "DefaultLoader.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"

#pragma unmanaged

void DefaultLoader::LoadTextureList(const simdjson::dom::array& texList, SMTextureList* v_tex_list)
{
	const int v_arrSz = static_cast<int>(texList.size());
	const int v_listSz = (v_arrSz > 3 ? 3 : v_arrSz);

	for (int a = 0; a < v_listSz; a++)
	{
		const auto v_curItem = texList.at(a);

		if (v_curItem.is_string())
		{
			std::wstring& v_wstrPath = v_tex_list->getStringRef(a);

			String::ToWideRef(v_curItem.get_string().value_unsafe(), v_wstrPath);
			KeywordReplacer::ReplaceKeyR(v_wstrPath);
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
	v_newEntry->m_material.assign(v_material.get_string().value_unsafe());

	const auto v_custom_prop = v_item["custom"];
	if (v_custom_prop.is_object())
	{
		const auto v_def_col_idx = v_custom_prop["color"];
		if (v_def_col_idx.is_string())
			v_newEntry->m_defColorIdx.assign(v_def_col_idx.get_string().value_unsafe());

		const auto v_shadow_only_mode = v_custom_prop["shadowOnly"];
		if (v_shadow_only_mode.is_bool())
			v_newEntry->m_shadowOnly = v_shadow_only_mode.get_bool().value_unsafe();
	}


	DefaultLoader::LoadTextureList(v_tex_list_obj.get_array().value_unsafe(), v_newEntry);

	(*v_sub_mesh) = v_newEntry;
	return true;
}

bool DefaultLoader::TryLoadSubMeshList(const simdjson::dom::element& lod_item, SMSubMeshBase** v_sub_mesh)
{
	const auto v_sub_mesh_list_obj = lod_item["subMeshList"];
	if (!v_sub_mesh_list_obj.is_array()) return false;

	std::size_t v_idx = 0;
	SMSubMeshList* v_subMeshList = new SMSubMeshList();

	for (const auto v_sub_mesh_item : v_sub_mesh_list_obj.get_array().value_unsafe())
	{
		if (!v_sub_mesh_item.is_object()) continue;

		const auto v_idxJson = v_sub_mesh_item["idx"];
		const std::size_t v_curIdx = (v_idxJson.is_number() ? JsonReader::GetNumber<std::size_t>(v_idxJson.value_unsafe()) : v_idx);

		SMTextureList* v_newTexData;
		if (DefaultLoader::LoadSubMeshDataEntry(v_sub_mesh_item, &v_newTexData))
			v_subMeshList->addTexList("", v_curIdx, v_newTexData);

		v_idx++;
	}

	(*v_sub_mesh) = v_subMeshList;
	return true;
}

bool DefaultLoader::TryLoadSubMeshMap(const simdjson::dom::element& lod_item, SMSubMeshBase** v_sub_mesh)
{
	const auto v_subMeshMapObj = lod_item["subMeshMap"];
	if (!v_subMeshMapObj.is_object()) return false;

	SMSubMeshMap* v_subMeshMap = new SMSubMeshMap();

	for (const auto v_subMeshItem : v_subMeshMapObj.get_object().value_unsafe())
	{
		if (!v_subMeshItem.value.is_object()) continue;

		SMTextureList* v_newTexData;
		if (DefaultLoader::LoadSubMeshDataEntry(v_subMeshItem.value, &v_newTexData))
			v_subMeshMap->addTexList(v_subMeshItem.key, 0, v_newTexData);
	}

	(*v_sub_mesh) = v_subMeshMap;
	return true;
}

bool DefaultLoader::LoadRenderableData(const simdjson::dom::element& jRenderable, SMSubMeshBase** tData, std::wstring& mesh)
{
	const auto v_lodListObj = jRenderable["lodList"];
	if (!v_lodListObj.is_array()) return false;

	const auto v_lodListArray = v_lodListObj.get_array().value_unsafe();
	if (v_lodListArray.size() == 0) return false;

	const auto v_lodList0 = v_lodListArray.at(0).value_unsafe();
	if (!v_lodList0.is_object()) return false;

	if (!DefaultLoader::TryLoadSubMeshList(v_lodList0, tData) &&
		!DefaultLoader::TryLoadSubMeshMap(v_lodList0, tData))
	{
		return false;
	}

	const auto v_meshPath = v_lodList0["mesh"];
	if (!v_meshPath.is_string()) return false;

	String::ToWideRef(v_meshPath.get_string().value_unsafe(), mesh);
	KeywordReplacer::ReplaceKeyR(mesh);

	return true;
}

bool DefaultLoader::LoadRenderableFromPath(const std::string_view& path, SMSubMeshBase** tData, std::wstring& mesh)
{
	std::wstring v_renderablePath(String::ToWide(path));
	KeywordReplacer::ReplaceKeyR(v_renderablePath);

	simdjson::dom::document v_rend_doc;
	if (!JsonReader::LoadParseSimdjsonCommentsC(v_renderablePath, v_rend_doc, simdjson::dom::element_type::OBJECT))
		return false;

	return DefaultLoader::LoadRenderableData(v_rend_doc.root(), tData, mesh);
}

bool DefaultLoader::LoadRenderable(const simdjson::dom::element& jAsset, SMSubMeshBase** tData, std::wstring& mesh)
{
	const auto v_rendData = jAsset["renderable"];
	if (v_rendData.error() != simdjson::SUCCESS)
		return false;

	switch (v_rendData.type())
	{
	case simdjson::dom::element_type::STRING:
		return DefaultLoader::LoadRenderableFromPath(v_rendData.get_string().value_unsafe(), tData, mesh);
	case simdjson::dom::element_type::OBJECT:
		return DefaultLoader::LoadRenderableData(v_rendData.value_unsafe(), tData, mesh);
	default:
		return false;
	}
}
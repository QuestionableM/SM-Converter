#pragma once

#include "ObjectDatabase/ObjectData.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Json.hpp"

SM_UNMANAGED_CODE

class DefaultLoader
{
public:
	static void LoadTextureList(const simdjson::dom::array& texList, SMTextureList* v_tex_list);
	static bool LoadSubMeshDataEntry(const simdjson::dom::element& v_item, SMTextureList** v_sub_mesh);
	static bool TryLoadSubMeshList(const simdjson::dom::element& lod_item, SMSubMeshBase** v_sub_mesh);
	static bool TryLoadSubMeshMap(const simdjson::dom::element& lod_item, SMSubMeshBase** v_sub_mesh);
	static bool LoadRenderableData(const simdjson::dom::element& jRenderable, SMSubMeshBase** tData, std::wstring& mesh);
	static bool LoadRenderableFromPath(const simdjson::dom::element& v_path, SMSubMeshBase** tData, std::wstring& mesh);
	static bool LoadRenderable(const simdjson::dom::element& jAsset, SMSubMeshBase** tData, std::wstring& mesh);
};

SM_MANAGED_CODE
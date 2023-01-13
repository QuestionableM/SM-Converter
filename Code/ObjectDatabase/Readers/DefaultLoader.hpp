#pragma once

#include "ObjectDatabase\ObjectData.hpp"
#include "Utils\Json.hpp"

class DefaultLoader
{
public:
	static TextureList LoadTextureList(const simdjson::dom::array& texList);
	static void AddSubMesh(const simdjson::dom::element& subMesh, TextureData& tData, const std::wstring& idx);
	static bool LoadTextureData(const simdjson::dom::element& jLodList, TextureData& tData);
	static bool LoadRenderableData(const simdjson::dom::element& jRenderable, TextureData& tData, std::wstring& mesh);
	static bool LoadRenderable(const simdjson::dom::element& jAsset, TextureData& tData, std::wstring& mesh);
};
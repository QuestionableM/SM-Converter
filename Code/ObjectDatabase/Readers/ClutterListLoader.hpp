#pragma once

#include "Utils\Json.hpp"

#pragma unmanaged

class Mod;
class ClutterData;
class SMTextureList;

class ClutterListLoader
{
	ClutterListLoader() = default;

	static bool LoadTextureData(const simdjson::dom::element& fClutter, SMTextureList& tList, std::wstring& mesh);
	static void LoadClutterData(const simdjson::dom::element& fClutter, ClutterData* pClutter);
public:
	static void Load(const simdjson::dom::element& fClutter, Mod* mod);
};

#pragma managed
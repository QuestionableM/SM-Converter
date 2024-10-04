#pragma once

#include "Utils/clr_include.hpp"
#include "Utils/Json.hpp"

SM_UNMANAGED_CODE

class SMMod;
class ClutterData;
class SMTextureList;
struct ClutterDataParams;

class ClutterListLoader
{
	ClutterListLoader() = default;

	static bool LoadTextureData(const simdjson::dom::element& fClutter, SMTextureList& tList, std::wstring& mesh);
	static ClutterDataParams LoadClutterData(const simdjson::dom::element& fClutter);

public:
	static void Load(const simdjson::dom::element& fClutter, SMMod* mod, bool add_to_global_db);
};

SM_MANAGED_CODE
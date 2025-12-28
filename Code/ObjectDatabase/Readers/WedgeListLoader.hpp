#pragma once

#include "Utils/clr_include.hpp"
#include "Utils/Json.hpp"

SM_UNMANAGED_CODE

class SMMod;
class SMTextureList;

class WedgeListLoader
{
public:
	static bool LoadTextureList(const simdjson::dom::element& wedge, SMTextureList& outTexList);

public:
	static void Load(const simdjson::dom::element& wedgeList, SMMod* pMod, bool addToGlobalDb);
};

SM_MANAGED_CODE
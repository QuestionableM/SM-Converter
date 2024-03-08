#pragma once

#include "ObjectDatabase/Mods/Mod.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Json.hpp"

SM_UNMANAGED_CODE

class DecalsetListReader
{
public:
	static void Load(const simdjson::dom::element& j_data, SMMod* mod, bool add_to_global_db);
};

SM_MANAGED_CODE
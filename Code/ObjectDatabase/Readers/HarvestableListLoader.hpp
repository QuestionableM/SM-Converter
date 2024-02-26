#pragma once

#include "Utils/clr_include.hpp"
#include "Utils/Json.hpp"
#include "Utils/Uuid.hpp"

SM_UNMANAGED_CODE

class SMMod;

class HarvestableListLoader
{
	HarvestableListLoader() = default;

public:
	static void Load(const simdjson::dom::element& fHarvestables, SMMod* mod, bool add_to_global_db);
};

SM_MANAGED_CODE
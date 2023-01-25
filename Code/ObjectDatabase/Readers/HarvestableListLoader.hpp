#pragma once

#include "Utils\Json.hpp"
#include "Utils\Uuid.hpp"

#pragma unmanaged

class SMMod;

class HarvestableListLoader
{
	HarvestableListLoader() = default;

	static bool CheckHarvestableExists(const SMUuid& v_uuid, SMMod* v_mod, const bool& add_to_global_db);

public:
	static void Load(const simdjson::dom::element& fHarvestables, SMMod* mod, const bool& add_to_global_db);
};

#pragma managed
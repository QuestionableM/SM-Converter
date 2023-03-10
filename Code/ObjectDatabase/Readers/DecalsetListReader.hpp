#pragma once

#include "ObjectDatabase\Mods\Mod.hpp"
#include "Utils\Json.hpp"

#pragma unmanaged

class DecalsetListReader
{
public:
	static void Load(const simdjson::dom::element& j_data, SMMod* mod, const bool& add_to_global_db);
};

#pragma managed
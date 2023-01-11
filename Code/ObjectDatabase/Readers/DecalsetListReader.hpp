#pragma once

#include "ObjectDatabase\Mods\Mod.hpp"
#include "Utils\Json.hpp"

class DecalsetListReader
{
public:
	static void Load(const simdjson::dom::element& j_data, Mod* mod);
};
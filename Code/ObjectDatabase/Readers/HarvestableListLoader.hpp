#pragma once

#include "Utils\Json.hpp"

#pragma unmanaged

class Mod;

class HarvestableListLoader
{
	HarvestableListLoader() = default;
public:
	static void Load(const simdjson::dom::element& fHarvestables, Mod* mod);
};

#pragma managed
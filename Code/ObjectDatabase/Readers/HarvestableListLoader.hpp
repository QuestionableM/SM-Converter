#pragma once

#include "Utils\Json.hpp"

#pragma unmanaged

class SMMod;

class HarvestableListLoader
{
	HarvestableListLoader() = default;
public:
	static void Load(const simdjson::dom::element& fHarvestables, SMMod* mod);
};

#pragma managed
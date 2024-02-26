#pragma once

#include "Utils/clr_include.hpp"
#include "Utils/Json.hpp"
#include "Utils/Uuid.hpp"

SM_UNMANAGED_CODE

class SMMod;
class SMTextureList;

class BlockListLoader
{
	BlockListLoader() = default;

	static bool GetBlockTextures(const simdjson::dom::element& block, SMTextureList& tex);
	static void GetBlockMaterial(const simdjson::dom::element& block, SMTextureList& tex);

public:
	static void Load(const simdjson::dom::element& fBlocks, SMMod* mod, bool add_to_global_db);
};

SM_MANAGED_CODE
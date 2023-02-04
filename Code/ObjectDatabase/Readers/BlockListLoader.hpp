#pragma once

#include "Utils\Json.hpp"
#include "Utils\Uuid.hpp"

#pragma unmanaged

class SMMod;
class SMTextureList;

class BlockListLoader
{
	BlockListLoader() = default;

	static bool GetBlockTextures(const simdjson::dom::element& block, SMTextureList& tex);
	static void GetBlockMaterial(const simdjson::dom::element& block, SMTextureList& tex);

public:
	static void Load(const simdjson::dom::element& fBlocks, SMMod* mod, const bool& add_to_global_db);
};

#pragma managed
#pragma once

#include "Utils\Json.hpp"

#pragma unmanaged

class Mod;
class SMTextureList;

class BlockListLoader
{
	BlockListLoader() = default;

	static bool GetBlockTextures(const simdjson::dom::element& block, SMTextureList& tex);
	static void GetBlockMaterial(const simdjson::dom::element& block, SMTextureList& tex);

public:
	static void Load(const simdjson::dom::element& fBlocks, Mod* mod);
};

#pragma managed
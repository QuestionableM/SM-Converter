#pragma once

#include "Utils\Json.hpp"

class Mod;
class TextureList;

class BlockListLoader
{
	BlockListLoader() = default;

	static bool GetBlockTextures(const simdjson::dom::element& block, TextureList& tex);
	static void GetBlockMaterial(const simdjson::dom::element& block, TextureList& tex);

public:
	static void Load(const simdjson::dom::element& fBlocks, Mod* mod);
};
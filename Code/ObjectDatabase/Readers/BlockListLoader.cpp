#include "BlockListLoader.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"
#include "ObjectDatabase\ProgCounter.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\Uuid.hpp"

#pragma unmanaged

static const std::string blkTexNames[3] = { "dif", "asg", "nor" };
bool BlockListLoader::GetBlockTextures(const simdjson::dom::element& block, SMTextureList& tex)
{
	for (int a = 0; a < 3; a++)
	{
		const auto v_curTex = block[blkTexNames[a]];

		if (v_curTex.is_string())
		{
			std::wstring& v_wstrRef = tex.getStringRef(a);

			v_wstrRef = String::ToWide(v_curTex.get_string());
			KeywordReplacer::ReplaceKeyR(v_wstrRef);
		}
	}

	return (!tex.dif.empty() || !tex.asg.empty() || !tex.nor.empty());
}

void BlockListLoader::GetBlockMaterial(const simdjson::dom::element& block, SMTextureList& tex)
{
	const auto v_glass = block["glass"];
	const auto v_alpha = block["alpha"];

	if (v_glass.is_bool() && v_glass.get_bool() == true)
	{
		tex.material = "BlockGlass";
	}
	else
	{
		tex.material = "BlockDifAsgNor";

		if (v_alpha.is_bool() && v_alpha.get_bool() == true)
			tex.material.append("Alpha");
	}
}

void BlockListLoader::Load(const simdjson::dom::element& fBlocks, SMMod* mod, bool add_to_global_db)
{
	if (!fBlocks.is_array()) return;

	const auto v_blk_array = fBlocks.get_array();
	ProgCounter::ProgressMax += v_blk_array.size();

	auto& v_cur_db = mod->m_Blocks.GetStorage(add_to_global_db);
	auto v_adder_func = mod->m_Blocks.GetAdderFunction(add_to_global_db);

	for (const auto v_blk : v_blk_array)
	{
		if (!v_blk.is_object()) continue;

		const auto v_uuid = v_blk["uuid"];
		const auto v_tiling = v_blk["tiling"];
		const auto v_color = v_blk["color"];

		if (!v_uuid.is_string()) continue;

		const SMUuid v_blk_uuid = v_uuid.get_c_str().value();
		if (mod->m_Blocks.ObjectExists(v_cur_db, v_blk_uuid))
			continue;

		SMTextureList v_tList;
		if (!BlockListLoader::GetBlockTextures(v_blk, v_tList)) continue;
		BlockListLoader::GetBlockMaterial(v_blk, v_tList);

		int v_blk_tiling = (v_tiling.is_number() ? JsonReader::GetNumber<int>(v_tiling) : 4);
		if (v_blk_tiling > 16 || v_blk_tiling <= 0) v_blk_tiling = 4;

		BlockData* v_new_blk = new BlockData();
		v_new_blk->m_uuid = v_blk_uuid;
		v_new_blk->m_textures = std::move(v_tList);
		v_new_blk->m_tiling = v_blk_tiling;
		v_new_blk->m_defaultColor = (v_color.is_string() ? v_color.get_c_str().value() : "375000");
		v_new_blk->m_mod = mod;

		(mod->m_Blocks.*v_adder_func)(v_new_blk);
		ProgCounter::ProgressValue++;
	}
}
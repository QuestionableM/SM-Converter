#include "BlockListLoader.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"
#include "ObjectDatabase\ProgCounter.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\Uuid.hpp"

SM_UNMANAGED_CODE

static const std::string_view g_blkTexNames[3] = { "dif", "asg", "nor" };
bool BlockListLoader::GetBlockTextures(const simdjson::dom::element& block, SMTextureList& tex)
{
	for (std::uint32_t a = 0; a < 3; a++)
	{
		const auto v_curTex = block[g_blkTexNames[a]];

		if (v_curTex.is_string())
		{
			std::wstring& v_wstrRef = tex.getStringRef(a);

			String::ToWideRef(v_curTex.get_string().value_unsafe(), v_wstrRef);
			KeywordReplacer::ReplaceKeyR(v_wstrRef);
		}
	}

	return (!tex.m_dif.empty() || !tex.m_asg.empty() || !tex.m_nor.empty());
}

void BlockListLoader::GetBlockMaterial(const simdjson::dom::element& block, SMTextureList& tex)
{
	const auto v_glass = block["glass"];
	const auto v_alpha = block["alpha"];

	if (v_glass.is_bool() && v_glass.get_bool().value_unsafe() == true)
	{
		tex.m_material = "BlockGlass";
	}
	else
	{
		tex.m_material = "BlockDifAsgNor";

		if (v_alpha.is_bool() && v_alpha.get_bool().value_unsafe() == true)
			tex.m_material.append("Alpha");
	}
}

void BlockListLoader::Load(const simdjson::dom::element& fBlocks, SMMod* mod, bool add_to_global_db)
{
	if (!fBlocks.is_array()) return;

	const auto v_blk_array = fBlocks.get_array().value_unsafe();
	ProgCounter::ProgressMax += v_blk_array.size();

	auto& v_cur_db = mod->m_Blocks.GetStorage(add_to_global_db);
	auto v_adder_func = mod->m_Blocks.GetAdderFunction(add_to_global_db);

	SMTextureList v_blkTexList;

	for (const auto v_blk : v_blk_array)
	{
		if (!v_blk.is_object()) continue;

		const auto v_uuid = v_blk["uuid"];
		const auto v_tiling = v_blk["tiling"];
		const auto v_color = v_blk["color"];

		if (!v_uuid.is_string()) continue;

		const SMUuid v_blkUuid = v_uuid.get_string().value_unsafe();
		if (mod->m_Blocks.ObjectExists(v_cur_db, v_blkUuid))
			continue;

		if (!BlockListLoader::GetBlockTextures(v_blk, v_blkTexList)) continue;
		BlockListLoader::GetBlockMaterial(v_blk, v_blkTexList);

		int v_blkTiling = (v_tiling.is_number() ? JsonReader::GetNumber<int>(v_tiling.value_unsafe()) : 4);
		if (v_blkTiling > 16 || v_blkTiling <= 0) v_blkTiling = 4;

		BlockData* v_pNewBlk = new BlockData(
			v_blkUuid,
			std::move(v_blkTexList),
			v_color.is_string() ? v_color.get_string().value_unsafe() : "375000",
			v_blkTiling,
			mod
		);

		(mod->m_Blocks.*v_adder_func)(v_pNewBlk);
		ProgCounter::ProgressValue++;
	}
}
#include "WedgeListLoader.hpp"

#include "ObjectDatabase/KeywordReplacer.hpp"
#include "ObjectDatabase/ProgCounter.hpp"
#include "ObjectDatabase/Mods/Mod.hpp"

SM_UNMANAGED_CODE

bool WedgeListLoader::LoadTextureList(
	const simdjson::dom::element& wedge,
	SMTextureList& outTexList)
{
	const auto v_material = wedge["material"];
	const auto v_texList = wedge["textureList"];

	if (!v_material.is_string() || !v_texList.is_array())
		return false;

	const auto v_texListArray = v_texList.get_array().value_unsafe();
	const std::size_t v_texCount = std::min(v_texListArray.size(), std::size_t(3));

	for (std::size_t a = 0; a < v_texCount; a++)
	{
		const auto v_curTex = v_texListArray.at(a);
		if (!v_curTex.is_string()) continue;

		std::wstring& v_wstrRef = outTexList.getStringRef(a);

		String::ToWideRef(v_curTex.get_string().value_unsafe(), v_wstrRef);
		KeywordReplacer::ReplaceKeyR(v_wstrRef);
	}

	outTexList.m_material = v_material.get_string().value_unsafe();
	return true;
}

void WedgeListLoader::Load(
	const simdjson::dom::element& wedgeList,
	SMMod* pMod,
	bool addToGlobalDb)
{
	if (!wedgeList.is_array()) return;

	const auto v_wedgeArray = wedgeList.get_array().value_unsafe();
	ProgCounter::ProgressMax += v_wedgeArray.size();

	auto& v_curDb = pMod->m_Wedges.GetStorage(addToGlobalDb);
	auto v_adderFunc = pMod->m_Wedges.GetAdderFunction(addToGlobalDb);

	// SMTextureList v_wedgeTexList;

	for (const auto v_curWedge : v_wedgeArray)
	{
		if (!v_curWedge.is_object()) continue;

		const auto v_blockUuid = v_curWedge["blockUuid"];
		if (!v_blockUuid.is_string()) continue;

		const auto v_pBlock = pMod->GetGlobalObject<BlockData>(v_blockUuid.get_string().value_unsafe());
		if (!v_pBlock)
		{
			DebugErrorL("Couldn't find a block for the specified wedge");
			continue;
		}

		const auto v_uuid = v_curWedge["uuid"];
		if (!v_uuid.is_string()) continue;

		const auto v_frameSize = v_curWedge["frameSize"];
		const auto v_defaultColor = v_curWedge["color"];

		const SMUuid v_wedgeUuid = v_uuid.get_string().value_unsafe();
		if (pMod->m_Wedges.ObjectExists(v_curDb, v_wedgeUuid))
			continue;

		//if (!WedgeListLoader::LoadTextureList(v_curWedge, v_wedgeTexList))
		//	continue;

		WedgeData* v_pNewWedge = new WedgeData(
			v_wedgeUuid,
			v_defaultColor.is_string() ? v_defaultColor.get_string().value_unsafe() : "375000",
			v_frameSize.is_number() ? JsonReader::GetNumber<float>(v_frameSize.value_unsafe()) : 0.0f,
			v_pBlock,
			pMod
		);

		(pMod->m_Wedges.*v_adderFunc)(v_pNewWedge);
		ProgCounter::ProgressValue++;
	}
}

SM_MANAGED_CODE
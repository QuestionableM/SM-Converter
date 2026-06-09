#include "ItemModCounter.hpp"

SM_UNMANAGED_CODE

ItemModInstance* ItemModStats::RegisterNewModDependency(
	const SMUuid& modUuid,
	const std::uint64_t modWorkshopId,
	const std::wstring_view& modPath,
	const std::wstring_view& modName)
{
	SMMod* v_pMod = SMMod::GetModFromUuid(modUuid);
	if (v_pMod)
		return ItemModStats::RegisterNewMod(v_pMod->GetUuid(), v_pMod->GetWorkshopId(), v_pMod->GetDirectory(), v_pMod->GetName());
	else
		return ItemModStats::RegisterNewMod(modUuid, modWorkshopId, modPath, modName);
}

ItemModInstance* ItemModStats::RegisterNewMod(
	const SMUuid& modUuid,
	const std::uint64_t modWorkshopId,
	const std::wstring_view& modPath,
	const std::wstring_view& modName)
{
	const auto v_iter = ItemModStats::ModStorage.find(modUuid);
	if (v_iter != ItemModStats::ModStorage.end())
		return v_iter->second;

	auto v_newInstance = new ItemModInstance(modUuid, modWorkshopId, modPath, modName);
	ItemModStats::ModVector.emplace_back(v_newInstance);
	ItemModStats::ModStorage.emplace(modUuid, v_newInstance);

	return v_newInstance;
}

ItemModInstance* ItemModStats::GetUnknownMod()
{
	return ItemModStats::RegisterNewMod(SMUuid::Null, 0, L"", L"UNKNOWN_MOD");
}

ItemModInstance* ItemModStats::FindModInstanceFromUuid(const SMUuid& partUuid)
{
	for (ItemModInstance* v_pCurMod : ItemModStats::ModVector)
		if (v_pCurMod->m_modParts.contains(partUuid)) return v_pCurMod;

	return nullptr;
}

void ItemModStats::CountPart(
	const SMUuid& partUuid,
	const bool checkBlocksAndParts)
{
	ItemModInstance* v_pModInstanceOverwrite = ItemModStats::FindModInstanceFromUuid(partUuid);
	ItemModInstance* v_pModInstance;

	SMMod* v_pMod;
	if (checkBlocksAndParts)
		v_pMod = SMMod::GetModFromBlocksAndParts(partUuid, true);
	else
		v_pMod = SMMod::GetModFromTileParts(partUuid);

	if (v_pMod)
	{
		v_pModInstance = ItemModStats::RegisterNewMod(v_pMod->GetUuid(), v_pMod->GetWorkshopId(), v_pMod->GetDirectory(), v_pMod->GetName());
		if (!v_pModInstanceOverwrite)
			v_pModInstanceOverwrite = v_pModInstance;
	}
	else
	{
		v_pModInstance = v_pModInstanceOverwrite;
	}

	// Get unknown mod instance
	if (!v_pModInstance)
		v_pModInstance = ItemModStats::GetUnknownMod();

	if (!v_pModInstanceOverwrite)
		v_pModInstanceOverwrite = ItemModStats::GetUnknownMod();

	v_pModInstance->m_partCount++;
	v_pModInstanceOverwrite->m_partCountOverride++;
}

std::size_t ItemModStats::GetTotalPartCount()
{
	std::size_t	v_output = 0;

	for (const ItemModInstance* v_curMod : ItemModStats::ModVector)
		v_output += v_curMod->m_partCount;

	return v_output;
}

void ItemModStats::Reset()
{
	ItemModStats::ModStorage.clear();
	ItemModStats::ModVector.clear();
}
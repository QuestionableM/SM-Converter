#pragma once

#include "UStd/UnmanagedUnorderedMap.hpp"
#include "UStd/UnmanagedVector.hpp"

#include "ObjectDatabase/Mods/Mod.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

struct ItemModInstance
{
	inline ItemModInstance(
		SMMod* pMod,
		std::size_t partCount
	) noexcept :
		m_pMod(pMod),
		m_partCount(partCount)
	{}

	SMMod* m_pMod;
	size_t m_partCount;
};

struct ItemModStats
{
	inline static std::unordered_map<SMUuid, ItemModInstance*> ModStorage = {};
	inline static std::vector<ItemModInstance*> ModVector = {};

	inline static void IncrementModPart(SMMod* pMod)
	{
		const SMUuid v_curUuid = (pMod != nullptr) ? pMod->GetUuid() : SMUuid::Null;

		const auto v_iter = ItemModStats::ModStorage.find(v_curUuid);
		if (v_iter == ItemModStats::ModStorage.end())
		{
			ItemModInstance* v_newInstance = new ItemModInstance(pMod, 1);

			ItemModStats::ModVector.emplace_back(v_newInstance);
			ItemModStats::ModStorage.emplace(v_curUuid, v_newInstance);
		}
		else
		{
			v_iter->second->m_partCount++;
		}
	}

	inline static std::size_t GetTotalPartCount()
	{
		std::size_t	v_output = 0;

		for (const ItemModInstance* v_curMod : ItemModStats::ModVector)
			v_output += v_curMod->m_partCount;

		return v_output;
	}

	inline static void Reset()
	{
		ItemModStats::ModStorage.clear();
		ItemModStats::ModVector.clear();
	}
};

SM_MANAGED_CODE
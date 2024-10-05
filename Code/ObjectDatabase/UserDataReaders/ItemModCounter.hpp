#pragma once

#include "UStd/UnmanagedUnorderedMap.hpp"
#include "UStd/UnmanagedVector.hpp"

#include "ObjectDatabase/Mods/Mod.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

struct ItemModInstance
{
	SMMod* mod;
	size_t part_count;
};

struct ItemModStats
{
	inline static std::unordered_map<SMUuid, ItemModInstance*> ModStorage = {};
	inline static std::vector<ItemModInstance*> ModVector = {};

	inline static void IncrementModPart(SMMod* v_mod)
	{
		const SMUuid v_cur_uuid = (v_mod != nullptr) ? v_mod->GetUuid() : SMUuid::Null;

		const auto v_iter = ItemModStats::ModStorage.find(v_cur_uuid);
		if (v_iter == ItemModStats::ModStorage.end())
		{
			ItemModInstance* v_new_instance = new ItemModInstance{ v_mod, 1 };

			ItemModStats::ModStorage.emplace(v_cur_uuid, v_new_instance);
			ItemModStats::ModVector.push_back(v_new_instance);
		}
		else
		{
			v_iter->second->part_count++;
		}
	}

	inline static std::size_t GetTotalPartCount()
	{
		std::size_t	v_output = 0;

		for (auto v_cur_mod : ItemModStats::ModVector)
			v_output += v_cur_mod->part_count;

		return v_output;
	}

	inline static void Reset()
	{
		for (auto v_cur_mod : ItemModStats::ModVector)
			delete v_cur_mod;

		ItemModStats::ModStorage.clear();
		ItemModStats::ModVector.clear();
	}
};

SM_MANAGED_CODE
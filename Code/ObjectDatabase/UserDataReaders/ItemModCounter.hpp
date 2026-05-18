#pragma once

#include "UStd/UnmanagedUnorderedMap.hpp"
#include "UStd/UnmanagedVector.hpp"

#include "ObjectDatabase/Mods/Mod.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

#include <unordered_set>

struct ItemModInstance
{
	inline ItemModInstance(
		const SMUuid& modUuid,
		const std::uint64_t modWorkshopId,
		const std::wstring_view& modPath,
		const std::wstring_view& modName,
		const std::size_t partCount
	) noexcept
		: m_modUuid(modUuid)
		, m_modWorkshopId(modWorkshopId)
		, m_modPath(modPath)
		, m_modName(modName)
		, m_modParts()
		, m_partCount(0)
	{}

	SMUuid m_modUuid;
	std::uint64_t m_modWorkshopId;
	std::wstring m_modPath;
	std::wstring m_modName;
	std::unordered_set<SMUuid> m_modParts;

	std::size_t m_partCount;
};

struct ItemModStats
{
	inline static std::unordered_map<SMUuid, ItemModInstance*> ModStorage = {};
	inline static std::vector<ItemModInstance*> ModVector = {};

	static ItemModInstance* RegisterNewModDependency(
		const SMUuid& modUuid,
		const std::uint64_t modWorkshopId,
		const std::wstring_view& modPath,
		const std::wstring_view& modName);
	static ItemModInstance* RegisterNewMod(
		const SMUuid& modUuid,
		const std::uint64_t modWorkshopId,
		const std::wstring_view& modPath,
		const std::wstring_view& modName);
	static ItemModInstance* FindModInstanceFromUuid(const SMUuid& partUuid);
	static void CountPart(const SMUuid& partUuid, const bool checkBlocksAndParts);

	static std::size_t GetTotalPartCount();
	static void Reset();
};

SM_MANAGED_CODE
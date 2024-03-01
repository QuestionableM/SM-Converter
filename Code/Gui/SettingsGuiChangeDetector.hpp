#pragma once

#include "UStd/UnmanagedUnorderedMap.hpp"
#include "UStd/UnmanagedString.hpp"
#include "UStd/UnmanagedVector.hpp"

#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

#include <unordered_set>

enum : unsigned char
{
	SettingsChangeDetector_LocalModList     = (1 << 0),
	SettingsChangeDetector_WorkshopModList  = (1 << 1),
	SettingsChangeDetector_UserItemFolder   = (1 << 2),
	SettingsChangeDetector_OpenLinksInSteam = (1 << 3),
	SettingsChangeDetector_GamePath         = (1 << 4)
};

class SettingsChangeDetector
{
public:
	SettingsChangeDetector();
	SettingsChangeDetector(const SettingsChangeDetector&&) = delete;
	SettingsChangeDetector(SettingsChangeDetector&&) = delete;
	SettingsChangeDetector(SettingsChangeDetector&) = delete;
	~SettingsChangeDetector() = default;

	static void RemoveFromCheckedVec(
		std::vector<std::wstring>& vec,
		std::unordered_set<std::wstring>& map,
		std::size_t idx);
	static void RemoveFromMap(
		std::unordered_set<std::wstring>& map,
		const std::wstring& path);

	inline void SetChangeBit(unsigned char bit, bool is_set)
	{
		if (is_set)
			m_changeData |= bit;
		else
			m_changeData &= ~bit;
	}

	inline bool IsAnyBitSet(const unsigned char& bit) const
	{
		return (m_changeData & bit) != 0;
	}

	inline bool HasAnyChanges() const
	{
		return (m_changeData != 0);
	}

	void UpdateChange(unsigned char op_id);
	void ApplyChanges();

	std::wstring m_gamePath = L"";

	std::vector<std::wstring> m_localModList = {};
	std::vector<std::wstring> m_workshopModList = {};
	std::unordered_set<std::wstring> m_modListMap = {};
	std::unordered_set<std::wstring> m_userItemFolders = {};
	
	bool m_openLinksInSteam = false;

private:
	unsigned char m_changeData;
};

SM_MANAGED_CODE
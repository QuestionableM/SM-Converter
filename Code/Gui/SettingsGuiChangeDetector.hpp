#pragma once

#include "UStd/UnmanagedUnorderedMap.hpp"
#include "UStd/UnmanagedString.hpp"
#include "UStd/UnmanagedVector.hpp"

#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

#include <unordered_set>

enum : std::uint8_t
{
	SettingsChangeDetector_LocalModList     = (1 << 0),
	SettingsChangeDetector_WorkshopModList  = (1 << 1),
	SettingsChangeDetector_UserItemFolder   = (1 << 2),
	SettingsChangeDetector_OpenLinksInSteam = (1 << 3),
	SettingsChangeDetector_GamePath         = (1 << 4),
	SettingsChangeDetector_WorkshopPath     = (1 << 5),
	SettingsChangeDetector_DarkMode         = (1 << 6)
};

class SettingsChangeDetector
{
public:
	SettingsChangeDetector();
	SettingsChangeDetector(const SettingsChangeDetector&&) = delete;
	SettingsChangeDetector(SettingsChangeDetector&&) = delete;
	SettingsChangeDetector(SettingsChangeDetector&) = delete;
	~SettingsChangeDetector() = default;

	static void RemoveFromMap(
		std::unordered_set<std::wstring>& map,
		const std::wstring& path);
	static void RemoveFromCheckedVec(
		std::vector<std::wstring>& vec,
		std::unordered_set<std::wstring>& map,
		const std::size_t idx);

	void SetChangeBit(const std::uint8_t bit, const bool isSet) noexcept;
	bool IsAnyBitSet(const std::uint8_t bit) const noexcept;
	bool HasAnyChanges() const noexcept;

	void UpdateChange(const std::uint8_t opId);
	void ApplyChanges();

	std::wstring m_gamePath;
	std::wstring m_workshopPath;

	std::vector<std::wstring> m_localModList;
	std::vector<std::wstring> m_workshopModList;
	std::unordered_set<std::wstring> m_modListMap;
	std::unordered_set<std::wstring> m_userItemFolders;
	
	bool m_openLinksInSteam;
	bool m_darkMode;

private:
	std::uint8_t m_changeData;
};

SM_MANAGED_CODE
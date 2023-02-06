#pragma once

#include "UStd\UnmanagedUnorderedMap.hpp"
#include "UStd\UnmanagedString.hpp"
#include "UStd\UnmanagedVector.hpp"

#pragma unmanaged

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

	static void RemoveFromCheckedVec(std::vector<std::wstring>& v_vec, std::unordered_map<std::wstring, unsigned char>& v_map, const std::size_t& v_idx);
	static void RemoveFromMap(std::unordered_map<std::wstring, unsigned char>& v_map, const std::wstring& v_path);

	std::wstring m_gamePath = L"";
	std::unordered_map<std::wstring, unsigned char> m_modListMap = {};
	std::vector<std::wstring> m_localModList    = {};
	std::vector<std::wstring> m_workshopModList = {};
	std::unordered_map<std::wstring, unsigned char> m_userItemFolders = {};

	bool m_openLinksInSteam = false;


	inline void SetChangeBit(const unsigned char& bit, const bool& is_set)
	{
		if (is_set)
			m_changeData |= bit;
		else
			m_changeData &= ~bit;
	}

	inline bool IsAnyBitSet(const unsigned char& bit)
	{
		return (m_changeData & bit) != 0;
	}

	inline bool HasAnyChanges() const
	{
		return (m_changeData != 0);
	}

	template<unsigned char t_option_id>
	inline constexpr void UpdateChange()
	{
		if constexpr (t_option_id == SettingsChangeDetector_LocalModList)
			this->SetChangeBit(t_option_id, m_localModList != DatabaseConfig::LocalModFolders);
		else if constexpr (t_option_id == SettingsChangeDetector_WorkshopModList)
			this->SetChangeBit(t_option_id, m_workshopModList != DatabaseConfig::ModFolders);
		else if constexpr (t_option_id == SettingsChangeDetector_UserItemFolder)
			this->SetChangeBit(t_option_id, m_userItemFolders != DatabaseConfig::UserItemFolders);
		else if constexpr (t_option_id == SettingsChangeDetector_OpenLinksInSteam)
			this->SetChangeBit(t_option_id, m_openLinksInSteam != DatabaseConfig::OpenLinksInSteam);
		else if constexpr (t_option_id == SettingsChangeDetector_GamePath)
			this->SetChangeBit(t_option_id, m_gamePath != DatabaseConfig::GamePath);
	}

	void ApplyChanges();

private:
	unsigned char m_changeData = 0x0;
};

#pragma managed
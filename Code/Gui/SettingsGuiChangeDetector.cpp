#include "SettingsGuiChangeDetector.hpp"

#include "ObjectDatabase\DatabaseConfig.hpp"

#pragma unmanaged

SettingsChangeDetector::SettingsChangeDetector()
{
	m_modListMap = DatabaseConfig::ModPathChecker;
	m_localModList = DatabaseConfig::LocalModFolders;
	m_workshopModList = DatabaseConfig::ModFolders;
	m_userItemFolders = DatabaseConfig::UserItemFolders;

	m_openLinksInSteam = DatabaseConfig::OpenLinksInSteam;

	m_gamePath = DatabaseConfig::GamePath;
}

void SettingsChangeDetector::RemoveFromCheckedVec(std::vector<std::wstring>& v_vec, std::unordered_map<std::wstring, unsigned char>& v_map, const std::size_t& v_idx)
{
	const auto v_iter = v_map.find(v_vec[v_idx]);
	if (v_iter != v_map.end())
		v_map.erase(v_iter);

	v_vec.erase(v_vec.begin() + v_idx);
}

void SettingsChangeDetector::RemoveFromMap(std::unordered_map<std::wstring, unsigned char>& v_map, const std::wstring& v_path)
{
	const auto v_iter = v_map.find(v_path);
	if (v_iter != v_map.end())
		v_map.erase(v_iter);
}

void SettingsChangeDetector::ApplyChanges()
{
	const bool v_local_mods_changed = m_changeData & SettingsChangeDetector_LocalModList;
	const bool v_workshop_mods_changed = m_changeData & SettingsChangeDetector_WorkshopModList;

	if (v_local_mods_changed || v_workshop_mods_changed)
	{
		DatabaseConfig::ModPathChecker = m_modListMap;

		if (v_local_mods_changed)
			DatabaseConfig::LocalModFolders = m_localModList;

		if (v_workshop_mods_changed)
			DatabaseConfig::ModFolders = m_workshopModList;
	}

	if (m_changeData & SettingsChangeDetector_UserItemFolder)
		DatabaseConfig::UserItemFolders = m_userItemFolders;

	if (m_changeData & SettingsChangeDetector_GamePath)
		DatabaseConfig::GamePath = m_gamePath;

	//Doesn't matter for boolean that much
	DatabaseConfig::OpenLinksInSteam = m_openLinksInSteam;

	m_changeData = 0x0;
}
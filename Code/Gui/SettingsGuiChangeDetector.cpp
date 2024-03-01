#include "SettingsGuiChangeDetector.hpp"

#include "ObjectDatabase/DatabaseConfig.hpp"

SM_UNMANAGED_CODE

SettingsChangeDetector::SettingsChangeDetector()
	: m_gamePath(DatabaseConfig::GamePath),
	m_localModList(DatabaseConfig::LocalModFolders),
	m_workshopModList(DatabaseConfig::ModFolders),
	m_modListMap(DatabaseConfig::ModPathChecker),
	m_userItemFolders(DatabaseConfig::UserItemFolders),
	m_openLinksInSteam(DatabaseConfig::OpenLinksInSteam),
	m_changeData(0) {}

void SettingsChangeDetector::RemoveFromCheckedVec(
	std::vector<std::wstring>& vec,
	std::unordered_set<std::wstring>& map,
	std::size_t idx)
{
	SettingsChangeDetector::RemoveFromMap(map, vec[idx]);
	vec.erase(vec.begin() + idx);
}

void SettingsChangeDetector::RemoveFromMap(
	std::unordered_set<std::wstring>& map,
	const std::wstring& path)
{
	const auto v_iter = map.find(path);
	if (v_iter != map.end())
		map.erase(v_iter);
}

void SettingsChangeDetector::UpdateChange(unsigned char op_id)
{
	switch (op_id)
	{
	case SettingsChangeDetector_LocalModList:
		this->SetChangeBit(op_id, m_localModList != DatabaseConfig::LocalModFolders);
		break;
	case SettingsChangeDetector_WorkshopModList:
		this->SetChangeBit(op_id, m_workshopModList != DatabaseConfig::ModFolders);
		break;
	case SettingsChangeDetector_UserItemFolder:
		this->SetChangeBit(op_id, m_userItemFolders != DatabaseConfig::UserItemFolders);
		break;
	case SettingsChangeDetector_OpenLinksInSteam:
		this->SetChangeBit(op_id, m_openLinksInSteam != DatabaseConfig::OpenLinksInSteam);
		break;
	case SettingsChangeDetector_GamePath:
		this->SetChangeBit(op_id, m_gamePath != DatabaseConfig::GamePath);
		break;
	}
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
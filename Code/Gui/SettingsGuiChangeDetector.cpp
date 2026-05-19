#include "SettingsGuiChangeDetector.hpp"

#include "ObjectDatabase/DatabaseConfig.hpp"

SM_UNMANAGED_CODE

SettingsChangeDetector::SettingsChangeDetector()
	: m_gamePath(DatabaseConfig::GamePath)
	, m_localModList(DatabaseConfig::LocalModFolders)
	, m_workshopModList(DatabaseConfig::ModFolders)
	, m_modListMap(DatabaseConfig::ModPathChecker)
	, m_userItemFolders(DatabaseConfig::UserItemFolders)
	, m_openLinksInSteam(DatabaseConfig::OpenLinksInSteam)
	, m_darkMode(DatabaseConfig::IsDarkMode)
	, m_changeData(0)
{}

void SettingsChangeDetector::RemoveFromMap(
	std::unordered_set<std::wstring>& map,
	const std::wstring& path)
{
	const auto v_iter = map.find(path);
	if (v_iter != map.end())
		map.erase(v_iter);
}

void SettingsChangeDetector::RemoveFromCheckedVec(
	std::vector<std::wstring>& vec,
	std::unordered_set<std::wstring>& map,
	const std::size_t idx)
{
	SettingsChangeDetector::RemoveFromMap(map, vec[idx]);
	vec.erase(vec.begin() + idx);
}

void SettingsChangeDetector::SetChangeBit(
	const std::uint8_t bit,
	const bool isSet) noexcept
{
	if (isSet)
		m_changeData |= bit;
	else
		m_changeData &= ~bit;
}

bool SettingsChangeDetector::IsAnyBitSet(
	const std::uint8_t bit) const noexcept
{
	return (m_changeData & bit) != 0;
}

bool SettingsChangeDetector::HasAnyChanges() const noexcept
{
	return m_changeData != 0;
}

void SettingsChangeDetector::UpdateChange(const std::uint8_t opId)
{
	switch (opId)
	{
	case SettingsChangeDetector_LocalModList:
		this->SetChangeBit(opId, m_localModList != DatabaseConfig::LocalModFolders);
		break;
	case SettingsChangeDetector_WorkshopModList:
		this->SetChangeBit(opId, m_workshopModList != DatabaseConfig::ModFolders);
		break;
	case SettingsChangeDetector_UserItemFolder:
		this->SetChangeBit(opId, m_userItemFolders != DatabaseConfig::UserItemFolders);
		break;
	case SettingsChangeDetector_OpenLinksInSteam:
		this->SetChangeBit(opId, m_openLinksInSteam != DatabaseConfig::OpenLinksInSteam);
		break;
	case SettingsChangeDetector_GamePath:
		this->SetChangeBit(opId, m_gamePath != DatabaseConfig::GamePath);
		break;
	case SettingsChangeDetector_WorkshopPath:
		this->SetChangeBit(opId, m_workshopPath != DatabaseConfig::WorkshopFolder);
		break;
	case SettingsChangeDetector_DarkMode:
		this->SetChangeBit(opId, m_darkMode != DatabaseConfig::IsDarkMode);
		break;
	}
}

void SettingsChangeDetector::ApplyChanges()
{
	const bool v_localModsChanged = m_changeData & SettingsChangeDetector_LocalModList;
	const bool v_workshopModsChanged = m_changeData & SettingsChangeDetector_WorkshopModList;

	if (v_localModsChanged || v_workshopModsChanged)
	{
		DatabaseConfig::ModPathChecker = m_modListMap;

		if (v_localModsChanged)
			DatabaseConfig::LocalModFolders = m_localModList;

		if (v_workshopModsChanged)
			DatabaseConfig::ModFolders = m_workshopModList;
	}

	if (m_changeData & SettingsChangeDetector_UserItemFolder)
		DatabaseConfig::UserItemFolders = m_userItemFolders;

	if (m_changeData & SettingsChangeDetector_GamePath)
		DatabaseConfig::GamePath = m_gamePath;

	if (m_changeData & SettingsChangeDetector_WorkshopPath)
		DatabaseConfig::WorkshopFolder = m_workshopPath;

	// Doesn't matter for boolean that much
	DatabaseConfig::OpenLinksInSteam = m_openLinksInSteam;
	DatabaseConfig::IsDarkMode = m_darkMode;

	m_changeData = 0x0;
}
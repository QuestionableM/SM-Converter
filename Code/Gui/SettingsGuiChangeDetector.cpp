#include "SettingsGuiChangeDetector.hpp"

#include "ObjectDatabase\DatabaseConfig.hpp"

#pragma unmanaged

SettingsChangeDetector::SettingsChangeDetector()
{
	m_modListMap = DatabaseConfig::ModPathChecker;
	m_localModList = DatabaseConfig::LocalModFolders;
	m_workshopModList = DatabaseConfig::ModFolders;
	m_blueprintFolders = DatabaseConfig::BlueprintFolders;
	m_tileFolders = DatabaseConfig::TileFolders;

	m_openLinksInSteam = DatabaseConfig::OpenLinksInSteam;
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

void SettingsChangeDetector::ApplyChanges() const
{
	DatabaseConfig::ModFolders = m_workshopModList;
	DatabaseConfig::LocalModFolders = m_localModList;
	DatabaseConfig::ModPathChecker = m_modListMap;

	DatabaseConfig::BlueprintFolders = m_blueprintFolders;
	DatabaseConfig::TileFolders = m_tileFolders;

	DatabaseConfig::OpenLinksInSteam = m_openLinksInSteam;
}

void SettingsChangeDetector::UpdateChange(const unsigned char& id)
{
	switch (id)
	{
	case SettingsChangeDetector_LocalModList:
		break;
	case SettingsChangeDetector_WorkshopModList:
		break;
	case SettingsChangeDetector_BlueprintFolders:
		break;
	case SettingsChangeDetector_TileFolders:
		break;
	case SettingsChangeDetector_OpenLinksInSteam:
		break;
	}
}
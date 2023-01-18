#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#pragma unmanaged

enum : unsigned char
{
	SettingsChangeDetector_LocalModList     = (1 << 0),
	SettingsChangeDetector_WorkshopModList  = (1 << 1),
	SettingsChangeDetector_BlueprintFolders = (1 << 2),
	SettingsChangeDetector_TileFolders      = (1 << 3),
	SettingsChangeDetector_OpenLinksInSteam = (1 << 4)
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

	std::unordered_map<std::wstring, unsigned char> m_modListMap = {};
	std::vector<std::wstring> m_localModList    = {};
	std::vector<std::wstring> m_workshopModList = {};
	std::unordered_map<std::wstring, unsigned char> m_blueprintFolders = {};
	std::unordered_map<std::wstring, unsigned char> m_tileFolders = {};

	bool m_openLinksInSteam = false;


	void ApplyChanges() const;

	void UpdateChange(const unsigned char& id);
	inline bool HasAnyChanges() const { return (m_changeData != 0); }

private:
	unsigned char m_changeData = 0x0;
};

#pragma managed
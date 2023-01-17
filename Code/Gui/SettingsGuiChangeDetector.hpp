#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#pragma unmanaged

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
};

#pragma managed
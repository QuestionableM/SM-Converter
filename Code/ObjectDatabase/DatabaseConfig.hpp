#pragma once

#include "Utils\Json.hpp"

#include <string>

class DatabaseConfig
{
public:
	constexpr static const std::wstring_view ConfigPath = L"./Resources/Config.json";
	constexpr static const std::wstring_view RotationsPath = L"./Resources/RotationSettings.json";
	constexpr static const std::wstring_view MaterialMapPath = L"./Resources/MaterialIds.json";
	constexpr static const std::wstring_view GroundTexturesPath = L"./Resources/GroundTextures.json";

	inline static std::wstring GamePath                          = L"";
	inline static std::vector<std::wstring> AssetListFolders     = {};
	inline static std::vector<std::wstring> ModFolders           = {};
	inline static std::vector<std::wstring> LocalModFolders      = {};
	inline static std::vector<std::wstring> ResourceUpgradeFiles = {};
	inline static std::vector<std::pair<std::wstring, std::wstring>> DefaultKeywords = {};

private:
	static void WstrArrayToJson(nlohmann::json& j_obj, const std::string& key, const std::vector<std::wstring>& r_wstr_vec);
	static void JsonStrArrayToVector(const nlohmann::json& pJson, const std::string& pKey, std::vector<std::wstring>& pWstrVec, const bool& replace_keys);
	static void AddToStrVec(std::vector<std::wstring>& mWstrVec, const std::wstring& mWstr);

	static void ReadProgramSettings(const nlohmann::json& config_json);

	static bool GetSteamPaths(std::wstring& r_game_path, std::wstring& r_workshop_path);
	static void FindLocalUsers();
	static void FindGamePath(const nlohmann::json& config_json, bool& should_write);
	static void ReadUserSettings(const nlohmann::json& config_json, bool& should_write);

	static nlohmann::json GetConfigJson(bool* should_write, const bool& read_from_file);
	static void AddKeywordReplacement(const std::wstring& key, const std::wstring& value);
	static void UpdatePathReplacement();

public:
	static void SaveConfig();
	static void ReadConfig();
};
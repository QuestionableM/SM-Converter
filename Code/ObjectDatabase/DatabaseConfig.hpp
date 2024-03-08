#pragma once

#include "UStd/UnmanagedString.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Json.hpp"

SM_UNMANAGED_CODE

#include <unordered_set>

class DatabaseConfig
{
public:
	//Used to remove the same path entires
	using PathChecker = std::unordered_set<std::wstring>;

	constexpr static const std::wstring_view BlueprintOutputFolder = L".\\ConvertedBlueprints";
	constexpr static const std::wstring_view TileOutputFolder = L".\\ConvertedTiles";
	constexpr static const std::wstring_view WorldOutputFolder = L".\\ConvertedWorlds";

	constexpr static const std::wstring_view ConfigPath = L"./Resources/Config.json";
	constexpr static const std::wstring_view RotationsPath = L"./Resources/RotationSettings.json";
	constexpr static const std::wstring_view MaterialMapPath = L"./Resources/MaterialIds.json";
	constexpr static const std::wstring_view GroundTexturesPath = L"./Resources/GroundTextures.json";

	inline static std::wstring GamePath = L"";
	inline static std::wstring WorkshopFolder = L"";

	inline static bool OpenLinksInSteam = false;
	inline static bool IsDarkMode = true;

	inline static PathChecker AssetListFolders     = {};
	inline static PathChecker UserItemFolders      = {};
	inline static PathChecker ResourceUpgradeFiles = {};

	inline static PathChecker ModPathChecker = {};
	inline static std::vector<std::wstring> ModFolders           = {};
	inline static std::vector<std::wstring> LocalModFolders      = {};

	inline static std::unordered_map<std::wstring, std::wstring> DefaultKeywords = {};

private:
	static void WstrVecToJson(nlohmann::json& j_obj, const std::string& key, const std::vector<std::wstring>& r_wstr_vec);
	static void WstrMapToJson(nlohmann::json& j_obj, const std::string& key, const PathChecker& v_map);

public:
	static bool AddToStrVec(std::vector<std::wstring>& v_vec, PathChecker& v_path_checker, const std::wstring& v_new_str);
	static bool AddToStrMap(PathChecker& v_map, const std::wstring& v_new_str);

private:
	static void JsonStrArrayToStrVec(const nlohmann::json& v_json, const std::string& key, std::vector<std::wstring>& v_vec, PathChecker& v_path_checker, bool replace_keys);
	static void JsonStrArrayToStrMap(const nlohmann::json& v_json, const std::string& key, PathChecker& v_map, bool replace_keys);

	static void ReadProgramSettings(const nlohmann::json& config_json);

	static bool GetSteamPaths(std::wstring& r_game_path, std::wstring& r_workshop_path);
	static void FindLocalUsers();
	static void FindGamePath(const nlohmann::json& config_json, bool& should_write);
	static bool ReadUserSettings(const nlohmann::json& config_json, bool& should_write);

	static nlohmann::json GetConfigJson(bool* should_write, bool read_from_file);
	static void AddKeywordReplacement(const std::wstring& key, const std::wstring& path);
	static void UpdateGamePathReplacement();

	static bool ReplaceKeyAndAddToMap(const std::wstring& path, PathChecker& v_map);
	static void FillUserItems(bool should_fill, bool& should_write);

public:
	static void SaveConfig();
	static void ReadConfig();
	static void ClearConfig();
};

SM_MANAGED_CODE
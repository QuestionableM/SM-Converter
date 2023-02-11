#include "DatabaseConfig.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"
#include "UStd\UnmanagedFilesystem.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

#include <valve_vdf\vdf_parser.hpp>

void DatabaseConfig::WstrVecToJson(nlohmann::json& j_obj, const std::string& key, const std::vector<std::wstring>& r_wstr_vec)
{
	nlohmann::json v_strArray = nlohmann::json::array();

	for (const std::wstring& v_curWstr : r_wstr_vec)
		v_strArray.push_back(String::ToUtf8(v_curWstr));

	j_obj[key] = v_strArray;
}

void DatabaseConfig::WstrMapToJson(nlohmann::json& j_obj, const std::string& key, const PathChecker& v_map)
{
	nlohmann::json v_strArray = nlohmann::json::array();

	for (const auto& v_item : v_map)
		v_strArray.push_back(String::ToUtf8(v_item.first));

	j_obj[key] = v_strArray;
}

bool DatabaseConfig::AddToStrVec(std::vector<std::wstring>& v_vec, PathChecker& v_path_checker, const std::wstring& v_new_str)
{
	std::wstring v_full_path;
	if (!File::GetFullFilePathLower(v_new_str, v_full_path))
		return false;

	if (v_full_path == DatabaseConfig::WorkshopFolder)
	{
		DebugWarningL("Cannot add a current workshop folder into the path list!");
		return false;
	}

	const auto v_iter = v_path_checker.find(v_full_path);
	if (v_iter != v_path_checker.end())
	{
		DebugWarningL("The specified path already exists: ", v_full_path);
		return false;
	}

	v_path_checker.insert(std::make_pair(v_full_path, 1));
	v_vec.push_back(v_full_path);

	return true;
}

bool DatabaseConfig::AddToStrMap(PathChecker& v_map, const std::wstring& v_new_str)
{
	std::wstring v_full_path;
	if (!File::GetFullFilePathLower(v_new_str, v_full_path))
		return false;

	if (v_full_path == DatabaseConfig::WorkshopFolder)
	{
		DebugWarningL("Cannot add a current workshop folder into the path list!");
		return false;
	}

	const auto v_iter = v_map.find(v_full_path);
	if (v_iter != v_map.end())
	{
		DebugWarningL("The specified path already exists: ", v_full_path);
		return false;
	}

	v_map.insert(std::make_pair(v_full_path, 1));
	return true;
}

void DatabaseConfig::JsonStrArrayToStrVec(const nlohmann::json& v_json, const std::string& key, std::vector<std::wstring>& v_vec, PathChecker& v_path_checker, const bool& replace_keys)
{
	const auto v_array = JsonReader::Get(v_json, key);
	if (!v_array.is_array()) return;

	for (const auto& v_cur_item : v_array)
	{
		if (!v_cur_item.is_string()) continue;

		std::wstring v_wstr_path = String::ToWide(v_cur_item.get_ref<const std::string&>());
		if (replace_keys)
			KeywordReplacer::ReplaceKeyR(v_wstr_path);

		DatabaseConfig::AddToStrVec(v_vec, v_path_checker, v_wstr_path);
	}
}

void DatabaseConfig::JsonStrArrayToStrMap(const nlohmann::json& v_json, const std::string& key, PathChecker& v_map, const bool& replace_keys)
{
	const auto v_array = JsonReader::Get(v_json, key);
	if (!v_array.is_array()) return;

	for (const auto& v_cur_item : v_array)
	{
		if (!v_cur_item.is_string()) continue;

		std::wstring v_wstr_path = String::ToWide(v_cur_item.get_ref<const std::string&>());
		if (replace_keys)
			KeywordReplacer::ReplaceKeyR(v_wstr_path);

		DatabaseConfig::AddToStrMap(v_map, v_wstr_path);
	}
}

void DatabaseConfig::ReadProgramSettings(const nlohmann::json& config_json)
{
	const auto& program_settings = JsonReader::Get(config_json, "ProgramSettings");
	if (!program_settings.is_object()) return;

	const auto& pKeywords = JsonReader::Get(program_settings, "Keywords");
	if (pKeywords.is_object())
	{
		for (const auto& key_obj : pKeywords.items())
		{
			const auto& v_keyObjVal = key_obj.value();
			if (!v_keyObjVal.is_string()) continue;

			const std::wstring v_key = String::ToWide(key_obj.key());

			std::wstring v_value = String::ToWide(v_keyObjVal.get_ref<const std::string&>());
			KeywordReplacer::ReplaceKeyR(v_value);

			DatabaseConfig::AddKeywordReplacement(v_key, v_value);
		}
	}

	DatabaseConfig::JsonStrArrayToStrMap(program_settings, "ResourceUpgradeFiles", DatabaseConfig::ResourceUpgradeFiles, true);
	KeywordReplacer::LoadResourceUpgradesFromConfig();

	DatabaseConfig::JsonStrArrayToStrMap(program_settings, "ScrapAssetDatabase", DatabaseConfig::AssetListFolders, true);
}

bool DatabaseConfig::GetSteamPaths(std::wstring& r_game_path, std::wstring& r_workshop_path)
{
	using vdf_childs_table = std::unordered_map<std::string, std::shared_ptr<tyti::vdf::object>>;
	using vdf_attrib_table = std::unordered_map<std::string, std::string>;

	std::wstring v_steamPath;
	if (!String::ReadRegistryKey(L"SOFTWARE\\Valve\\Steam", L"SteamPath", v_steamPath))
	{
		if (!String::ReadRegistryKey(L"SOFTWARE\\WOW6432Node\\Valve\\Steam", L"SteamPath", v_steamPath))
		{
			return false;
		}
	}

	if (!File::Exists(v_steamPath))
		return false;

	DebugOutL("Found a steam path: ", v_steamPath);

	const std::wstring v_guessedGamePath = v_steamPath + L"/steamapps/common/scrap mechanic";
	const std::wstring v_guessedWorkshopPath = v_steamPath + L"/steamapps/workshop/content/387990";

	if (File::Exists(v_guessedGamePath) && File::Exists(v_guessedWorkshopPath))
	{
		r_game_path = v_guessedGamePath;
		r_workshop_path = v_guessedWorkshopPath;

		DebugOutL("Guessed the game path: ", 0b1101_fg, v_guessedGamePath);
		DebugOutL("Guessed the workshop path: ", 0b1101_fg, v_guessedWorkshopPath);

		return true;
	}
	else
	{
		const std::wstring v_vdfPath = v_steamPath + L"/steamapps/libraryfolders.vdf";
		if (!File::Exists(v_vdfPath))
			return false;

		DebugOutL("VDF Path found: ", v_vdfPath);

		std::ifstream v_vdfReader(v_vdfPath);
		if (!v_vdfReader.is_open())
			return false;

		tyti::vdf::basic_object<char> v_vdfRoot = tyti::vdf::read(v_vdfReader);
		v_vdfReader.close();

		if (v_vdfRoot.name != "libraryfolders")
			return false;

		const vdf_childs_table& v_vdfLibFolders = v_vdfRoot.childs;
		for (const auto& v_libFolder : v_vdfLibFolders)
		{
			const vdf_attrib_table& v_libFolderAttribs = v_libFolder.second->attribs;
			const vdf_attrib_table::const_iterator v_attribIter = v_libFolderAttribs.find("path");
			if (v_attribIter == v_libFolderAttribs.end())
				continue;

			const vdf_childs_table& v_libFolderChilds = v_libFolder.second->childs;
			const vdf_childs_table::const_iterator v_childsIter = v_libFolderChilds.find("apps");
			if (v_childsIter == v_libFolderChilds.end())
				continue;

			const vdf_attrib_table& v_appAttribs = v_childsIter->second->attribs;
			const vdf_attrib_table::const_iterator v_appAttribsIter = v_appAttribs.find("387990");
			if (v_appAttribsIter == v_appAttribs.end())
				continue;

			const std::wstring v_libraryPathWstr = String::ToWide(v_attribIter->second);
			const std::wstring v_librarySmPath = v_libraryPathWstr + L"/steamapps/common/scrap mechanic";
			const std::wstring v_libraryWsPath = v_libraryPathWstr + L"/steamapps/workshop/content/387990";

			const bool v_librarySmExists = File::Exists(v_librarySmPath);
			const bool v_libraryWsExists = File::Exists(v_libraryWsPath);

			if (v_librarySmExists)
			{
				DebugOutL("Found a game path from the library: ", 0b1101_fg, v_librarySmPath);
				r_game_path = v_librarySmPath;
			}

			if (v_libraryWsExists)
			{
				DebugOutL("Found a workshop path from the library: ", 0b1101_fg, v_libraryWsPath);
				r_workshop_path = v_libraryWsPath;
			}

			if (v_librarySmExists || v_libraryWsExists)
			{
				DebugOutL("Successfully got the steam paths!");
				return true;
			}
		}
	}

	return false;
}

void DatabaseConfig::FindLocalUsers()
{
	namespace fs = std::filesystem;

	std::wstring v_smLocalData;
	if (!File::GetAppDataPath(v_smLocalData))
		return;

	v_smLocalData.append(L"\\Axolot Games\\Scrap Mechanic\\User");
	if (!File::Exists(v_smLocalData))
		return;

	std::error_code v_errorCode;
	fs::directory_iterator v_dirIterator(v_smLocalData, fs::directory_options::skip_permission_denied, v_errorCode);

	for (const auto& v_curDir : v_dirIterator)
	{
		if (v_errorCode)
		{
			DebugErrorL("Couldn't read an item in the directory: ", v_smLocalData);
			continue;
		}

		if (!v_curDir.is_directory()) continue;

		const std::wstring v_cur_dir_path = v_curDir.path().wstring();

		const std::wstring v_local_mods_dir = v_cur_dir_path + L"\\Mods";
		if (File::Exists(v_local_mods_dir))
		{
			DebugOutL("Found a new path to local mods: ", 0b01101_fg, v_local_mods_dir);
			DatabaseConfig::AddToStrVec(DatabaseConfig::LocalModFolders, DatabaseConfig::ModPathChecker, v_local_mods_dir);
		}

		const std::wstring v_local_blueprints_dir = v_cur_dir_path + L"\\Blueprints";
		if (File::Exists(v_local_blueprints_dir))
		{
			DebugOutL("Found a new path to local blueprints: ", 0b01101_fg, v_local_blueprints_dir);
			DatabaseConfig::AddToStrMap(DatabaseConfig::UserItemFolders, v_local_blueprints_dir);
		}

		const std::wstring v_local_tiles_dir = v_cur_dir_path + L"\\Tiles";
		if (File::Exists(v_local_tiles_dir))
		{
			DebugOutL("Found a new path to local tiles: ", 0b01101_fg, v_local_tiles_dir);
			DatabaseConfig::AddToStrMap(DatabaseConfig::UserItemFolders, v_local_tiles_dir);
		}

		const std::wstring v_local_worlds_dir = v_cur_dir_path + L"\\Worlds";
		if (File::Exists(v_local_worlds_dir))
		{
			DebugOutL("Found a new path to local worlds: ", 0b01101_fg, v_local_worlds_dir);
			DatabaseConfig::AddToStrMap(DatabaseConfig::UserItemFolders, v_local_worlds_dir);
		}
	}
}

void DatabaseConfig::FindGamePath(const nlohmann::json& config_json, bool& should_write)
{
	if (
		(DatabaseConfig::GamePath.empty() || !File::Exists(DatabaseConfig::GamePath)) ||
		(DatabaseConfig::WorkshopFolder.empty() || !File::Exists(DatabaseConfig::WorkshopFolder)))
	{
		std::wstring game_path, ws_path;
		if (DatabaseConfig::GetSteamPaths(game_path, ws_path))
		{
			DatabaseConfig::GamePath = game_path;
			DatabaseConfig::WorkshopFolder = ws_path;

			should_write = true;
		}

		DatabaseConfig::FindLocalUsers();
	}
}

inline void ReadJsonDirectory(const nlohmann::json& config_json, const std::string& key, std::wstring& v_output)
{
	const auto& v_dir_json = JsonReader::Get(config_json, key);
	if (!v_dir_json.is_string())
		return;

	std::wstring v_dir = String::ToWide(v_dir_json.get_ref<const std::string&>());
	if (!File::GetFullFilePathLower(v_dir, v_dir))
	{
		DebugErrorL("Couldn't get the full path for: ", v_dir);
		return;
	}

	if (File::IsDirectory(v_dir))
	{
		v_output = v_dir;
	}
	else
	{
		DebugErrorL("The specified path is not a directory!");
	}
}

bool DatabaseConfig::ReadUserSettings(const nlohmann::json& config_json, bool& should_write)
{
	const auto& user_settings = JsonReader::Get(config_json, "UserSettings");
	if (user_settings.is_object())
	{
		ReadJsonDirectory(user_settings, "GameDirectory", DatabaseConfig::GamePath);
		ReadJsonDirectory(user_settings, "WorkshopDirectory", DatabaseConfig::WorkshopFolder);

		const auto& v_open_in_steam = JsonReader::Get(user_settings, "OpenLinksInSteam");
		DatabaseConfig::OpenLinksInSteam = v_open_in_steam.is_boolean() ? v_open_in_steam.get<bool>() : false;

		DatabaseConfig::JsonStrArrayToStrVec(user_settings, "LocalModFolders", DatabaseConfig::LocalModFolders, DatabaseConfig::ModPathChecker, false);
		DatabaseConfig::JsonStrArrayToStrVec(user_settings, "ModFolders", DatabaseConfig::ModFolders, DatabaseConfig::ModPathChecker, false);

		DatabaseConfig::JsonStrArrayToStrMap(user_settings, "UserItemFolders", DatabaseConfig::UserItemFolders, false);
	}

	DatabaseConfig::FindGamePath(config_json, should_write);

	return !user_settings.contains("UserItemFolders");
}

nlohmann::json DatabaseConfig::GetConfigJson(bool* should_write, const bool& read_from_file)
{
	nlohmann::json cfgData;
	if (read_from_file)
	{
		cfgData = JsonReader::LoadParseJson(DatabaseConfig::ConfigPath.data());
		if (!cfgData.is_object())
			cfgData = nlohmann::json::object();
	}
	else
	{
		cfgData = nlohmann::json::object();
	}

	nlohmann::json v_programSettings = JsonReader::Get(cfgData, "ProgramSettings");
	if (!v_programSettings.is_object())
		v_programSettings = nlohmann::json::object();

	if (!v_programSettings.contains("Keywords"))
	{
		v_programSettings["Keywords"] =
		{
			{ "$CHALLENGE_DATA", "$GAME_FOLDER/ChallengeData" },
			{ "$GAME_DATA"     , "$GAME_FOLDER/Data"          },
			{ "$SURVIVAL_DATA" , "$GAME_FOLDER/Survival"      }
		};

		if (should_write)
			(*should_write) = true;
	}

	if (!v_programSettings.contains("ResourceUpgradeFiles"))
	{
		v_programSettings["ResourceUpgradeFiles"] =
		{
			"$GAME_DATA/upgrade_resources.json"
		};

		if (should_write)
			(*should_write) = true;
	}

	if (!v_programSettings.contains("ScrapAssetDatabase"))
	{
		v_programSettings["ScrapAssetDatabase"] =
		{
			"$CHALLENGE_DATA/Terrain/Database/AssetSets",
			"$SURVIVAL_DATA/Terrain/Database/AssetSets",
			"$GAME_DATA/Terrain/Database/AssetSets",

			"$SURVIVAL_DATA/Harvestables/Database/HarvestableSets",

			"$CHALLENGE_DATA/Objects/Database/ShapeSets",
			"$GAME_DATA/Objects/Database/ShapeSets",
			"$SURVIVAL_DATA/Objects/Database/ShapeSets",

			"$GAME_DATA/Terrain/Database/clutter.json",
			"$SURVIVAL_DATA/Terrain/Database/clutter.json",

			"$SURVIVAL_DATA/Decals/decalsets.json"
		};

		if (should_write)
			(*should_write) = true;
	}

	cfgData["ProgramSettings"] = v_programSettings;

	return cfgData;
}

void DatabaseConfig::AddKeywordReplacement(const std::wstring& key, const std::wstring& path)
{
	DebugOutL(key, " is set to ", path);

	DatabaseConfig::DefaultKeywords[key] = path;
	KeywordReplacer::SetReplacement(key, path);
}

void DatabaseConfig::UpdateGamePathReplacement()
{
	DatabaseConfig::AddKeywordReplacement(L"$GAME_FOLDER", DatabaseConfig::GamePath);
}

bool DatabaseConfig::ReplaceKeyAndAddToMap(const std::wstring& path, PathChecker& v_map)
{
	const std::wstring v_replaced_path = KeywordReplacer::ReplaceKey(path);
	if (!File::Exists(v_replaced_path)) return false;

	return DatabaseConfig::AddToStrMap(v_map, v_replaced_path);
}

void DatabaseConfig::FillUserItems(const bool& should_fill, bool& should_write)
{
	if (!should_fill) return;

	DebugOutL("Filling user items with default paths...");

	const static wchar_t* v_tile_paths[] =
	{
		L"$SURVIVAL_DATA/Terrain/Tiles/autumn_forest",
		L"$SURVIVAL_DATA/Terrain/Tiles/burnt_forest",
		L"$SURVIVAL_DATA/Terrain/Tiles/desert",
		L"$SURVIVAL_DATA/Terrain/Tiles/field",
		L"$SURVIVAL_DATA/Terrain/Tiles/forest",
		L"$SURVIVAL_DATA/Terrain/Tiles/lake",
		L"$SURVIVAL_DATA/Terrain/Tiles/meadow",
		L"$SURVIVAL_DATA/Terrain/Tiles/poi",
		L"$SURVIVAL_DATA/Terrain/Tiles/roads_and_cliffs",
		L"$SURVIVAL_DATA/Terrain/Tiles/start_area",

		L"$GAME_DATA/Terrain/Tiles/ClassicCreativeTiles",
		L"$GAME_DATA/Terrain/Tiles/CreativeTiles",

		L"$SURVIVAL_DATA/DungeonTiles"
	};

	constexpr const std::size_t v_tile_paths_sz = sizeof(v_tile_paths) / sizeof(wchar_t*);
	for (std::size_t a = 0; a < v_tile_paths_sz; a++)
		should_write |= DatabaseConfig::ReplaceKeyAndAddToMap(v_tile_paths[a], DatabaseConfig::UserItemFolders);
}

void DatabaseConfig::SaveConfig()
{
	nlohmann::json cfgData = DatabaseConfig::GetConfigJson(nullptr, false);

	{
		nlohmann::json user_settings = nlohmann::json::object();

		user_settings["GameDirectory"] = String::ToUtf8(DatabaseConfig::GamePath);
		user_settings["WorkshopDirectory"] = String::ToUtf8(DatabaseConfig::WorkshopFolder);

		user_settings["OpenLinksInSteam"] = DatabaseConfig::OpenLinksInSteam;

		DatabaseConfig::WstrVecToJson(user_settings, "ModFolders", DatabaseConfig::ModFolders);
		DatabaseConfig::WstrVecToJson(user_settings, "LocalModFolders", DatabaseConfig::LocalModFolders);
		DatabaseConfig::WstrMapToJson(user_settings, "UserItemFolders", DatabaseConfig::UserItemFolders);

		cfgData["UserSettings"] = user_settings;
	}

	DebugOutL(0b0110_fg, "Saving a new config...");
	JsonReader::WriteJson(DatabaseConfig::ConfigPath.data(), cfgData);

	//Update the game path keyword in case the path was updated
	DatabaseConfig::UpdateGamePathReplacement();
}

void DatabaseConfig::ReadConfig()
{
	DebugOutL(0b0110_fg, "Reading the program config...");

	DatabaseConfig::ClearConfig();

	bool should_write = false;
	nlohmann::json v_cfg_data = DatabaseConfig::GetConfigJson(&should_write, true);

	const bool v_fill_user_items = DatabaseConfig::ReadUserSettings(v_cfg_data, should_write);
	
	//Stop reading the config if the path to the game is invalid
	if (DatabaseConfig::GamePath.empty()) return;

	DatabaseConfig::UpdateGamePathReplacement();
	DatabaseConfig::ReadProgramSettings(v_cfg_data);

	DatabaseConfig::FillUserItems(v_fill_user_items, should_write);

	if (should_write)
	{
		DatabaseConfig::SaveConfig();
	}
}

void DatabaseConfig::ClearConfig()
{
	DatabaseConfig::GamePath.clear();
	DatabaseConfig::OpenLinksInSteam = false;

	DatabaseConfig::AssetListFolders.clear();
	DatabaseConfig::UserItemFolders.clear();
	DatabaseConfig::ResourceUpgradeFiles.clear();

	DatabaseConfig::ModPathChecker.clear();
	DatabaseConfig::LocalModFolders.clear();
	DatabaseConfig::ModFolders.clear();

	DatabaseConfig::DefaultKeywords.clear();
	KeywordReplacer::Clear();
}
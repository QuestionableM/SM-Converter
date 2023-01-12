#include "ObjectDatabase.hpp"

#include "ObjectDatabase\GroundTextureDatabase.hpp"
#include "ObjectDatabase\DatabaseConfig.hpp"
#include "ObjectDatabase\ProgCounter.hpp"

#include "ObjectDatabase\Mods\GameDataMod.hpp"
#include "ObjectDatabase\MaterialManager.hpp"
#include "ObjectDatabase\ObjectRotations.hpp"
#include "ObjectDatabase\KeywordReplacer.hpp"

#include "Utils\Console.hpp"
#include "Utils\Json.hpp"

#include <filesystem>

#pragma unmanaged

void DatabaseLoader::LoadGameDatabase()
{
	ProgCounter::SetState(ProgState::LoadingVanilla, 0);
	DebugOutL(0b0010_fg, "Loading game data...");

	Mod* pGameData = new GameDataMod();
	pGameData->LoadObjectDatabase();

	Mod::ModVector.push_back(pGameData);
}

void DatabaseLoader::LoadModsFromDirectory(const std::wstring& mod_directory, const bool& is_local)
{
	namespace fs = std::filesystem;

	std::error_code v_error_code;
	fs::directory_iterator v_dir_iter(mod_directory, fs::directory_options::skip_permission_denied, v_error_code);

	for (const auto& v_cur_item : v_dir_iter)
	{
		if (v_error_code || !v_cur_item.is_directory())
			continue;

		Mod* v_new_mod = Mod::LoadFromDescription(v_cur_item.path().wstring(), is_local);
		if (!v_new_mod) continue;

		v_new_mod->LoadObjectDatabase();
	}
}

void DatabaseLoader::LoadModsFromPaths(const std::vector<std::wstring>& path_vector, const bool& is_local)
{
	namespace fs = std::filesystem;

	for (const std::wstring& v_mod_dir : path_vector)
		DatabaseLoader::LoadModsFromDirectory(v_mod_dir, is_local);
}

void DatabaseLoader::LoadModDatabase()
{
	ProgCounter::SetState(ProgState::LoadingModded);
	DebugOutL(0b0100_fg, "Loading mod data...");

	DatabaseLoader::LoadModsFromPaths(DatabaseConfig::LocalModFolders, true);
	DatabaseLoader::LoadModsFromPaths(DatabaseConfig::ModFolders, false);
}

void DatabaseLoader::LoadDatabase()
{
	Mod::ClearModStorage();

	DatabaseLoader::LoadGameDatabase();
	DatabaseLoader::LoadModDatabase();

	KeywordReplacer::ClearContentKey();

	DebugOutL(0b0101_fg, "Finished! (Blocks: ", Mod::BlockStorage.size(), ", Parts: ", Mod::PartStorage.size(), ", Harvestables: ", Mod::HarvestableStorage.size(), ", Assets: ", Mod::AssetStorage.size(), ")");
}

void DatabaseLoader::InitializeDatabase()
{
	DatabaseConfig::ReadConfig();
	MaterialManager::Initialize();
	GroundTextureDatabase::Initialize();
	Rotations::InitializeRotations();
}
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
namespace fs = std::filesystem;

void DatabaseLoader::LoadGameDatabase()
{
	ProgCounter::SetState(ProgState::LoadingVanilla, 0);
	DebugOutL(0b0010_fg, "Loading game data...");

	Mod* pGameData = new GameDataMod();
	pGameData->LoadObjectDatabase();

	Mod::ModVector.push_back(pGameData);
}

void DatabaseLoader::LoadModsFromPaths(const std::vector<std::wstring>& path_vector, const bool& is_local)
{
	for (const std::wstring& v_modDir : path_vector)
	{
		std::error_code v_errorCode;
		fs::directory_iterator v_dirIter(v_modDir, fs::directory_options::skip_permission_denied, v_errorCode);

		for (const auto& v_curDir : v_dirIter)
		{
			if (v_errorCode || !v_curDir.is_directory())
				continue;

			Mod* v_newMod = Mod::LoadFromDescription(v_curDir.path().wstring(), is_local);
			if (!v_newMod) continue;

			v_newMod->LoadObjectDatabase();
		}
	}
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
#include "ObjectDatabase.hpp"

#include "ObjectDatabase\GroundTextureDatabase.hpp"
#include "ObjectDatabase\DatabaseConfig.hpp"
#include "ObjectDatabase\ProgCounter.hpp"

#include "ObjectDatabase\Mods\GameDataMod.hpp"
#include "ObjectDatabase\MaterialManager.hpp"
#include "ObjectDatabase\ObjectRotations.hpp"
#include "ObjectDatabase\KeywordReplacer.hpp"

#include "UStd\UnmanagedFilesystem.hpp"

#include "Utils\Console.hpp"
#include "Utils\Json.hpp"

#pragma unmanaged

void DatabaseLoader::LoadGameDatabase()
{
	ProgCounter::SetState(ProgState::LoadingVanilla, 0);
	DebugOutL(0b0010_fg, "Loading game data...");

	SMMod* v_game_data = new GameDataMod();
	v_game_data->LoadObjectDatabase();

	SMMod::GameDataMod = v_game_data;
}

void DatabaseLoader::LoadModsFromPath(const std::wstring& path, const bool& is_local)
{
	namespace fs = std::filesystem;

	std::error_code v_errorCode;
	fs::directory_iterator v_dirIter(path, fs::directory_options::skip_permission_denied, v_errorCode);

	for (const auto& v_curDir : v_dirIter)
	{
		if (v_errorCode || !v_curDir.is_directory())
			continue;

		SMMod* v_newMod = SMMod::LoadFromDescription(v_curDir.path().wstring(), is_local);
		if (!v_newMod) continue;

		v_newMod->LoadObjectDatabase();
	}
}

void DatabaseLoader::LoadModsFromPaths(const std::vector<std::wstring>& path_vector, const bool& is_local)
{
	for (const std::wstring& v_modDir : path_vector)
		DatabaseLoader::LoadModsFromPath(v_modDir, is_local);
}

void DatabaseLoader::LoadModDatabase()
{
	ProgCounter::SetState(ProgState::LoadingModded);
	DebugOutL(0b0100_fg, "Loading mod data...");

	DatabaseLoader::LoadModsFromPaths(DatabaseConfig::LocalModFolders, true);
	DatabaseLoader::LoadModsFromPath(DatabaseConfig::WorkshopFolder, false);
	DatabaseLoader::LoadModsFromPaths(DatabaseConfig::ModFolders, false);
}

void DatabaseLoader::LoadDatabase()
{
	SMMod::ClearModStorage();

	DatabaseLoader::LoadGameDatabase();
	DatabaseLoader::LoadModDatabase();

	KeywordReplacer::ClearModKeys();

	DebugOutL(0b0101_fg, "Successfully loaded ", SMMod::GetAmountOfObjects(), " objects!");
}

void DatabaseLoader::InitializeDatabase()
{
	DatabaseConfig::ReadConfig();
	MaterialManager::Initialize();
	GroundTextureDatabase::Initialize();
	Rotations::InitializeRotations();
}
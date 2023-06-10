#include "GameDataMod.hpp"

#include "ObjectDatabase\DatabaseConfig.hpp"

#include "UStd\UnmanagedFilesystem.hpp"

#pragma unmanaged

GameDataMod::GameDataMod()
	: SMMod(
		L"Vanilla Data",
		DatabaseConfig::GamePath,
		SMUuid(0x45157449ee614269, 0xa6ab3137e2945fca),
		0,
		true) {}

void GameDataMod::LoadObjectDatabase()
{
	namespace fs = std::filesystem;

	this->LoadFile(L"./Resources/ResourceReplacements.json", true);

	for (const auto& db_item : DatabaseConfig::AssetListFolders)
	{
		std::error_code v_error;
		const bool is_regular_file = fs::is_regular_file(db_item.first, v_error);
		if (v_error) continue;

		if (is_regular_file)
			this->LoadFile(db_item.first, true);
		else
			this->ScanDatabaseFolderRecursive(db_item.first, true);
	}
}
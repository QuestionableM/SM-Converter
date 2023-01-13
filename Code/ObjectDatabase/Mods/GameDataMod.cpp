#include "GameDataMod.hpp"

#include "ObjectDatabase\DatabaseConfig.hpp"

#include <filesystem>
namespace fs = std::filesystem;

GameDataMod::GameDataMod()
{
	this->m_Name = L"Vanilla Data";
	this->m_Directory = DatabaseConfig::GamePath;
	this->m_Uuid = SMUuid();
}

void GameDataMod::LoadObjectDatabase()
{
	for (const std::wstring& db_dir : DatabaseConfig::AssetListFolders)
	{
		std::error_code rError;
		const bool is_regular_file = fs::is_regular_file(db_dir, rError);

		if (rError) continue;

		if (is_regular_file)
		{
			this->LoadFile(db_dir);
		}
		else
		{
			this->ScanDatabaseFolderRecursive(db_dir);
		}
	}
}
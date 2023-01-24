#include "GameDataMod.hpp"

#include "ObjectDatabase\DatabaseConfig.hpp"

#include "UStd\UnmanagedFilesystem.hpp"

#pragma unmanaged

GameDataMod::GameDataMod()
{
	this->m_Name = L"Vanilla Data";
	this->m_Directory = DatabaseConfig::GamePath;
	this->m_Uuid = SMUuid(0x45157449ee614269, 0xa6ab3137e2945fca);
	this->m_WorkshopId = 0;
}

void GameDataMod::LoadObjectDatabase()
{
	namespace fs = std::filesystem;

	for (const auto& db_item : DatabaseConfig::AssetListFolders)
	{
		std::error_code v_error;
		const bool is_regular_file = fs::is_regular_file(db_item.first, v_error);
		if (v_error) continue;

		if (is_regular_file)
			this->LoadFile(db_item.first);
		else
			this->ScanDatabaseFolderRecursive(db_item.first);
	}
}
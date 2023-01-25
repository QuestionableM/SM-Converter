#include "TerrainAssetsMod.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

static const std::wstring g_AssetSetDirectoryPaths[2] = { L"/Terrain/Database/", L"/Database/" };
bool TerrainAssetsMod::GetValidAssetDatabaseFolder(const std::wstring& mod_path, std::wstring& r_asset_db)
{
	for (__int8 a = 0; a < 2; a++)
	{
		const std::wstring asset_db_path = mod_path + g_AssetSetDirectoryPaths[a];

		if (File::Exists(asset_db_path))
		{
			r_asset_db = asset_db_path;
			return true;
		}
	}

	return false;
}

static const std::wstring g_AssetSetDbExtensions[2] = { L"json", L"assetdb" };
bool TerrainAssetsMod::GetAssetSetDatabaseFile(const std::wstring& asset_db_dir, std::wstring& r_asset_set)
{
	const std::wstring near_full_path = asset_db_dir + L"assetsets.";

	for (__int8 a = 0; a < 2; a++)
	{
		const std::wstring full_db_path = near_full_path + g_AssetSetDbExtensions[a];

		if (File::Exists(full_db_path))
		{
			r_asset_set = full_db_path;
			return true;
		}
	}

	return false;
}

void TerrainAssetsMod::LoadObjectDatabase()
{
	KeywordReplacer::SetModData(m_Directory, m_Uuid);

	std::wstring v_database_dir;
	if (!TerrainAssetsMod::GetValidAssetDatabaseFolder(m_Directory, v_database_dir))
	{
		DebugErrorL("Couldn't find a valid asset database directory!");
		return;
	}

	std::wstring v_assetdb_path;
	if (TerrainAssetsMod::GetAssetSetDatabaseFile(v_database_dir, v_assetdb_path))
	{
		SMMod::LoadAssetSetList(v_assetdb_path, this, true);
	}
	else
	{
		const std::wstring v_asset_sets_folder = v_database_dir + L"AssetSets";
		if (!File::Exists(v_asset_sets_folder)) return;

		this->ScanDatabaseFolder(v_asset_sets_folder, true);
	}
}
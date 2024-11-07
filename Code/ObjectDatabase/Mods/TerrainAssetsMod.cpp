#include "TerrainAssetsMod.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

TerrainAssetsMod::TerrainAssetsMod(
	const std::wstring& name,
	const std::wstring& directory,
	const SMUuid& uuid,
	std::uint64_t workshopId,
	bool isLocal
) :
	SMMod(
		name,
		directory,
		uuid,
		workshopId,
		isLocal
	)
{
	SMMod::ModStorage.emplace(m_Uuid, this);
	SMMod::ModVector.push_back(this);
}

static const std::wstring_view g_assetSetDirectoryPaths[2] = { L"/Terrain/Database/", L"/Database/" };
bool TerrainAssetsMod::GetValidAssetDatabaseFolder(const std::wstring& mod_path, std::wstring& r_asset_db)
{
	std::wstring v_assetDbPath;

	for (std::uint8_t a = 0; a < 2; a++)
	{
		v_assetDbPath = mod_path;
		v_assetDbPath.append(g_assetSetDirectoryPaths[a]);
		if (!File::Exists(v_assetDbPath)) continue;

		r_asset_db = v_assetDbPath;
		return true;
	}

	return false;
}

static const std::wstring_view g_assetSetDbExtensions[2] = { L"json", L"assetdb" };
bool TerrainAssetsMod::GetAssetSetDatabaseFile(const std::wstring& asset_db_dir, std::wstring& r_asset_set)
{
	const std::wstring v_nearFullPath = asset_db_dir + L"assetsets.";
	std::wstring v_fullDbPath;

	for (std::uint8_t a = 0; a < 2; a++)
	{
		v_fullDbPath = v_nearFullPath;
		v_fullDbPath.append(g_assetSetDbExtensions[a]);
		if (!File::Exists(v_fullDbPath)) continue;

		r_asset_set = v_fullDbPath;
		return true;
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
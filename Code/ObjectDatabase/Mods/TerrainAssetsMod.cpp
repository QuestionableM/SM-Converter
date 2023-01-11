#include "TerrainAssetsMod.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\File.hpp"

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

void TerrainAssetsMod::LoadAssetSetDatabase(const std::wstring& path, Mod* pMod)
{
	simdjson::dom::document v_assetset_doc;
	if (!JsonReader::LoadParseSimdjsonCommentsC(path, v_assetset_doc, simdjson::dom::element_type::OBJECT))
		return;

	const auto v_assetset_list = v_assetset_doc.root()["assetSetList"];
	if (!v_assetset_list.is_array()) return;

	for (const auto v_asset_set : v_assetset_list.get_array())
	{
		if (!v_asset_set.is_object()) continue;

		const auto v_assetset_path_obj = v_asset_set["assetSet"];
		if (!v_assetset_path_obj.is_string()) continue;

		std::wstring v_assetset_path = String::ToWide(v_assetset_path_obj.get_string());
		KeywordReplacer::ReplaceKeyR(v_assetset_path);

		pMod->LoadFile(v_assetset_path);
	}
}

void TerrainAssetsMod::LoadObjectDatabase()
{
	KeywordReplacer::SetModData(m_Directory, m_Uuid);

	std::wstring l_AssetDatabaseDir;
	if (!TerrainAssetsMod::GetValidAssetDatabaseFolder(m_Directory, l_AssetDatabaseDir))
	{
		DebugErrorL("Couldn't find a valid asset database directory!");
		return;
	}

	std::wstring l_AssetSetDatabasePath;
	if (TerrainAssetsMod::GetAssetSetDatabaseFile(l_AssetDatabaseDir, l_AssetSetDatabasePath))
	{
		TerrainAssetsMod::LoadAssetSetDatabase(l_AssetSetDatabasePath, this);
	}
	else
	{
		const std::wstring l_AssetSetsFolder = l_AssetDatabaseDir + L"AssetSets";
		if (!File::Exists(l_AssetSetsFolder)) return;

		this->ScanDatabaseFolder(l_AssetSetsFolder);
	}
}
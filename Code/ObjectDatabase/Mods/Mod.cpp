#include "Mod.hpp"

#include "ObjectDatabase\Readers\HarvestableListLoader.hpp"
#include "ObjectDatabase\Readers\KinematicListLoader.hpp"
#include "ObjectDatabase\Readers\DecalsetListReader.hpp"
#include "ObjectDatabase\Readers\ClutterListLoader.hpp"
#include "ObjectDatabase\Readers\GarmentListLoader.hpp"
#include "ObjectDatabase\Readers\AssetListLoader.hpp"
#include "ObjectDatabase\Readers\BlockListLoader.hpp"
#include "ObjectDatabase\Readers\PartListLoader.hpp"
#include "ObjectDatabase\KeywordReplacer.hpp"

#include "ObjectDatabase\Mods\BlocksAndPartsMod.hpp"
#include "ObjectDatabase\Mods\TerrainAssetsMod.hpp"
#include "ObjectDatabase\Mods\CustomGameMod.hpp"

#include "UStd\UnmanagedFilesystem.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

void SMMod::ClearModStorage()
{
	SMModObjectStorage<BlockData>::Clear();
	SMModObjectStorage<PartData>::Clear();
	SMModObjectStorage<AssetData>::Clear();
	SMModObjectStorage<HarvestableData>::Clear();
	SMModObjectStorage<ClutterData>::Clear();
	SMModObjectStorage<DecalData>::Clear();
	SMModObjectStorage<KinematicData>::Clear();

	SMMod::ClutterVector.clear();

	{
		//Clear all the garments
		for (const auto v_category : SMMod::GarmentStorage)
			for (const auto v_item : v_category.second)
				delete v_item.second;

		SMMod::GarmentStorage.clear();
	}

	{
		//Remove all the mods from the memory
		if (SMMod::GameDataMod)
		{
			delete SMMod::GameDataMod;
			SMMod::GameDataMod = nullptr;
		}

		for (std::size_t a = 0; a < SMMod::ModVector.size(); a++)
			delete SMMod::ModVector[a];

		for (std::size_t a = 0; a < SMMod::CustomGameVector.size(); a++)
			delete SMMod::CustomGameVector[a];

		SMMod::CustomGameStorage.clear();
		SMMod::CustomGameVector.clear();

		SMMod::ModStorage.clear();
		SMMod::ModVector.clear();
	}
}

inline bool GetModTypeFromString(const char* v_str, ModType& v_type_ref)
{
	if (strcmp(v_str, "Blocks and Parts") == 0)
	{
		v_type_ref = ModType::BlocksAndParts;
		return true;
	}
	else if (strcmp(v_str, "Terrain Assets") == 0)
	{
		v_type_ref = ModType::TerrainAssets;
		return true;
	}
	else if (strcmp(v_str, "Custom Game") == 0)
	{
		v_type_ref = ModType::CustomGame;
		return true;
	}

	return false;
}

SMMod* SMMod::LoadFromDescription(const std::wstring& mod_folder, const bool& is_local)
{
	const std::wstring mDescPath = mod_folder + L"/description.json";
	if (!File::Exists(mDescPath)) return nullptr;

	simdjson::dom::document v_doc;
	if (!JsonReader::LoadParseSimdjsonCommentsC(mDescPath, v_doc, simdjson::dom::element_type::OBJECT))
		return nullptr;

	const auto v_root = v_doc.root();

	const auto v_mod_type_obj = v_root["type"];
	if (!v_mod_type_obj.is_string()) return nullptr;

	ModType v_mod_type;
	if (!GetModTypeFromString(v_mod_type_obj.get_c_str(), v_mod_type))
		return nullptr;

	const auto v_mod_uuid_obj = v_root["localId"];
	const auto v_mod_name_obj = v_root["name"];

	if (!(v_mod_uuid_obj.is_string() && v_mod_name_obj.is_string()))
		return nullptr;

	const SMUuid v_mod_uuid = v_mod_uuid_obj.get_c_str();
	const std::wstring v_mod_name = String::ToWide(v_mod_name_obj.get_c_str());

	const std::unordered_map<SMUuid, SMMod*>& v_cur_map = (v_mod_type != ModType::CustomGame) ? SMMod::ModStorage : SMMod::CustomGameStorage;
	const std::unordered_map<SMUuid, SMMod*>::const_iterator v_iter = v_cur_map.find(v_mod_uuid);
	if (v_iter != v_cur_map.end())
	{
	#if defined(DEBUG) || defined(_DEBUG)
		const SMMod* v_modPtr = v_iter->second;

		if (v_iter->second->m_isLocal && !is_local)
		{
			DebugOutL("Skipped a mod as it was already loaded locally (name: ", v_mod_name, ", uuid: ", v_mod_uuid.ToString(), ")");
			return nullptr;
		}

		if (v_iter->second->m_isLocal == is_local)
		{
			DebugWarningL("Uuid conflict between: ", v_iter->second->m_Name, " and ", v_mod_name, " (uuid: ", v_mod_uuid.ToString(), ")");
			return nullptr;
		}
	#else
		return nullptr;
	#endif
	}

	const auto v_mod_ws_id = v_root["fileId"];
	const unsigned long long v_mod_workshop_id = v_mod_ws_id.is_number() ? JsonReader::GetNumber<unsigned long long>(v_mod_ws_id) : 0ull;

	SMMod* v_new_mod;
	switch (v_mod_type)
	{
	case ModType::BlocksAndParts:
		v_new_mod = new BlocksAndPartsMod(v_mod_name, mod_folder, v_mod_uuid, v_mod_workshop_id, is_local);
		break;
	case ModType::TerrainAssets:
		v_new_mod = new TerrainAssetsMod(v_mod_name, mod_folder, v_mod_uuid, v_mod_workshop_id, is_local);
		break;
	case ModType::CustomGame:
		{
			CustomGame* v_cg_mod = new CustomGame(v_mod_name, mod_folder, v_mod_uuid, v_mod_workshop_id, is_local);

			const auto v_allow_add_mods = v_root["allow_add_mods"];
			v_cg_mod->m_shouldUseUserMods = (v_allow_add_mods.is_bool() ? v_allow_add_mods.get_bool() : true);

			v_new_mod = v_cg_mod;
			break;
		}
	default:
		return nullptr;
	}

	DebugOutL("Mod: ", 0b1101_fg, v_new_mod->m_Name, 0b1110_fg, ", Uuid: ", 0b1101_fg, v_new_mod->m_Uuid.ToString(), 0b1110_fg, ", Type: ", 0b1101_fg, v_mod_type_obj.get_c_str().value_unsafe());
	return v_new_mod;
}

CustomGame* SMMod::GetCustomGameFromPath(const std::wstring& v_path)
{
	for (CustomGame* v_cur_cg : SMMod::CustomGameVector)
	{
		if (File::IsSubPath(v_cur_cg->GetDirectory(), v_path))
			return v_cur_cg;
	}

	return nullptr;
}

using DataLoaderMap = std::unordered_map<std::string, void (*)(const simdjson::dom::element&, SMMod*, const bool&)>;
static const DataLoaderMap g_DataLoaders =
{
	{ "assetListRenderable", AssetListLoader::Load       },
	{ "harvestableList"    , HarvestableListLoader::Load },
	{ "partList"           , PartListLoader::Load		 },
	{ "blockList"          , BlockListLoader::Load		 },
	{ "clutterList"        , ClutterListLoader::Load     },
	{ "decalSetList"       , DecalsetListReader::Load    },
	{ "kinematicList"      , KinematicListLoader::Load   },
	{ "categoryList"       , GarmentListLoader::Load     }
};

void SMMod::LoadFile(const std::wstring& path, const bool& add_to_global_db)
{
	simdjson::dom::document v_doc;
	if (!JsonReader::LoadParseSimdjsonCommentsC(path, v_doc, simdjson::dom::element_type::OBJECT))
		return;

	DebugOutL("Loading: ", path);

	for (const auto v_obj : v_doc.root().get_object())
	{
		const std::string v_key_str = std::string(v_obj.key.data(), v_obj.key.size());

		const DataLoaderMap::const_iterator v_iter = g_DataLoaders.find(v_key_str);
		if (v_iter == g_DataLoaders.end())
		{
			DebugErrorL("Couldn't find the loader for: ", v_key_str);
			continue;
		}

		v_iter->second(v_obj.value, this, add_to_global_db);
	}
}

void SMMod::LoadAssetSetList(const std::wstring& path, SMMod* v_mod, const bool& add_to_global_db)
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

		v_mod->LoadFile(v_assetset_path, add_to_global_db);
	}
}

void SMMod::LoadShapeSetList(const std::wstring& path, SMMod* v_mod, const bool& add_to_global_db)
{
	simdjson::dom::document v_shapedb_doc;
	if (!JsonReader::LoadParseSimdjsonCommentsC(path, v_shapedb_doc, simdjson::dom::element_type::OBJECT))
		return;

	const auto v_shapeset_list = v_shapedb_doc.root()["shapeSetList"];
	if (!v_shapeset_list.is_array()) return;

	for (const auto v_shapeset : v_shapeset_list.get_array())
	{
		if (!v_shapeset.is_string()) continue;

		std::wstring v_shapeset_path = String::ToWide(v_shapeset.get_string());
		KeywordReplacer::ReplaceKeyR(v_shapeset_path);

		v_mod->LoadFile(v_shapeset_path, add_to_global_db);
	}
}

void SMMod::LoadHarvestableSetList(const std::wstring& path, SMMod* v_mod, const bool& add_to_global_db)
{
	simdjson::dom::document v_hvsdb_doc;
	if (!JsonReader::LoadParseSimdjsonCommentsC(path, v_hvsdb_doc, simdjson::dom::element_type::OBJECT))
		return;

	const auto v_hvsset_list = v_hvsdb_doc.root()["harvestableSetList"];
	if (!v_hvsset_list.is_array()) return;

	for (const auto v_hvsset : v_hvsset_list.get_array())
	{
		if (!v_hvsset.is_object()) continue;

		const auto v_hvsset_path_obj = v_hvsset["name"];
		if (!v_hvsset_path_obj.is_string()) continue;

		std::wstring v_hvsset_path = String::ToWide(v_hvsset_path_obj.get_string());
		KeywordReplacer::ReplaceKeyR(v_hvsset_path);

		v_mod->LoadFile(v_hvsset_path, add_to_global_db);
	}
}

void SMMod::LoadKinematicSetList(const std::wstring& path, SMMod* v_mod, const bool& add_to_global_db)
{
	simdjson::dom::document v_kinematicdb_doc;
	if (!JsonReader::LoadParseSimdjsonCommentsC(path, v_kinematicdb_doc, simdjson::dom::element_type::OBJECT))
		return;

	const auto v_kinematicset_list = v_kinematicdb_doc.root()["kinematicSetList"];
	if (!v_kinematicset_list.is_array()) return;

	for (const auto v_kinematic_set : v_kinematicset_list.get_array())
	{
		if (!v_kinematic_set.is_object()) continue;

		const auto v_kinematicset_path_obj = v_kinematic_set["name"];
		if (!v_kinematicset_path_obj.is_string()) continue;

		std::wstring v_kinematicset_path = String::ToWide(v_kinematicset_path_obj.get_string());
		KeywordReplacer::ReplaceKeyR(v_kinematicset_path);

		v_mod->LoadFile(v_kinematicset_path, add_to_global_db);
	}
}

inline bool IsShapeSetExtensionValid(const std::string& extension)
{
	if (extension == ".json") return true;
	if (extension == ".shapeset") return true;
	if (extension == ".assetset") return true;

	return false;
}

void SMMod::ScanDatabaseFolderRecursive(const std::wstring& folder, const bool& add_to_global_db)
{
	namespace fs = std::filesystem;

	std::error_code rError;
	fs::recursive_directory_iterator rDirIter(folder, fs::directory_options::skip_permission_denied, rError);

	for (const auto& dir : rDirIter)
	{
		if (rError || !dir.is_regular_file()) continue;

		const fs::path& dPath = dir.path();

		if (dPath.has_extension() && IsShapeSetExtensionValid(dPath.extension().string()))
			this->LoadFile(dPath.wstring(), add_to_global_db);
	}
}

void SMMod::ScanDatabaseFolder(const std::wstring& folder, const bool& add_to_global_db)
{
	namespace fs = std::filesystem;

	std::error_code rError;
	fs::directory_iterator rDirIter(folder, fs::directory_options::skip_permission_denied, rError);

	for (const auto& cur_dir : rDirIter)
	{
		if (rError || !cur_dir.is_regular_file()) continue;

		const fs::path& dir_path = cur_dir.path();

		if (dir_path.has_extension() && IsShapeSetExtensionValid(dir_path.extension().string()))
			this->LoadFile(dir_path.wstring(), add_to_global_db);
	}
}

void SMMod::SetContentKey() const
{
	KeywordReplacer::SetModData(m_Directory, m_Uuid);
}
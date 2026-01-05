#include "UserObjectFolderReader.hpp"

#include "BlueprintFolderReader.hpp"
#include "UserCharacterReader.hpp"
#include "WorldFolderReader.hpp"
#include "TileFolderReader.hpp"

#include "ObjectDatabase/DatabaseConfig.hpp"
#include "UStd/UnmanagedFilesystem.hpp"

void UserObjectFolderReader::ReadItemFromDirectory(const std::wstring& path)
{
	const std::wstring v_description_path = path + L"/description.json";

	simdjson::dom::document v_item_doc;
	if (!JsonReader::LoadParseSimdjsonCommentsC(v_description_path, v_item_doc, simdjson::dom::element_type::OBJECT))
		return;

	const auto v_doc_root = v_item_doc.root();
	const auto v_desc_type = v_doc_root["type"];

	if (v_desc_type.is_string())
	{
		const std::string_view v_descTypeView = v_desc_type.get_string().value_unsafe();

		if (v_descTypeView == "Tile")
			TileFolderReader::LoadFromFolder(path, v_doc_root);
		else if (v_descTypeView == "Blueprint")
			BlueprintFolderReader::LoadFromFolder(path, v_doc_root);
		else if (v_descTypeView == "World")
			WorldFolderReader::LoadFromFolder(path, v_doc_root);
	}
	else
	{
		//if it doesn't have a type, then we can assume it is an old blueprint
		BlueprintFolderReader::LoadFromFolder(path, v_doc_root);
	}
}

void UserObjectFolderReader::LoadObjectsFromFolder(const std::wstring& folder)
{
	namespace fs = std::filesystem;

	std::error_code v_error_code;
	fs::directory_iterator v_dir_iter(folder, fs::directory_options::skip_permission_denied, v_error_code);
	for (const auto& v_cur_item : v_dir_iter)
	{
		if (v_error_code) continue;

		if (v_cur_item.is_directory())
		{
			UserObjectFolderReader::ReadItemFromDirectory(v_cur_item.path().wstring());
		}
		else if (v_cur_item.is_regular_file())
		{
			const fs::path& v_item_path = v_cur_item.path();

			if (!v_item_path.has_extension())
				continue;

			const std::string v_item_extension = v_item_path.extension().string();
			if (v_item_extension == ".tile")
				TileFolderReader::LoadFromFile(v_item_path);
			else if (v_item_extension == ".blueprint")
				BlueprintFolderReader::LoadFromFile(v_item_path);
			else if (v_item_extension == ".world")
				WorldFolderReader::LoadFromFile(v_item_path);
		}
	}
}

void UserObjectFolderReader::ReadFromConfig()
{
	BlueprintFolderReader::ClearStorage();
	WorldFolderReader::ClearStorage();
	TileFolderReader::ClearStorage();
	UserCharacterReader::ClearStorage();

	for (const auto& v_item_folder : DatabaseConfig::UserItemFolders)
		UserObjectFolderReader::LoadObjectsFromFolder(v_item_folder);

	UserObjectFolderReader::LoadObjectsFromFolder(DatabaseConfig::WorkshopFolder);
	UserCharacterReader::LoadCharacters();

	DebugOutL("[UserObjectFolderReader] Successfully loaded ", BlueprintFolderReader::Storage.size(), " blueprints and ", TileFolderReader::Storage.size(), " tiles from ", DatabaseConfig::UserItemFolders.size(), " folders");
}
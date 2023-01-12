#include "BlueprintFolderReader.hpp"

#include "ObjectDatabase\DatabaseConfig.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\Json.hpp"
#include "Utils\File.hpp"

#include <filesystem>

#pragma unmanaged

void BlueprintFolderReader::ReadBlueprintFromFolder(const std::wstring& folder)
{
	const std::wstring v_description_path = folder + L"/description.json";
	if (!File::Exists(v_description_path))
		return;

	simdjson::dom::document v_bp_doc;
	if (!JsonReader::LoadParseSimdjsonCommentsC(v_description_path, v_bp_doc, simdjson::dom::element_type::OBJECT))
		return;

	const auto v_root = v_bp_doc.root();
	const auto v_bp_type = v_root["type"];
	if (!(v_bp_type.is_string() && strcmp(v_bp_type.get_c_str(), "Blueprint") == 0))
		return;

	const auto v_bp_name = v_root["name"];
	const auto v_bp_uuid = v_root["localId"];
	if (!(v_bp_name.is_string() && v_bp_uuid.is_string()))
		return;

	BlueprintInstance* v_new_bp = new BlueprintInstance();
	v_new_bp->name = String::ToWide(v_bp_name.get_string());
	v_new_bp->lower_name = String::ToLower(v_new_bp->name);
	v_new_bp->path = folder;

	const auto v_bp_workshop_id = v_root["fileId"];
	v_new_bp->workshop_id = (v_bp_workshop_id.is_number() ? JsonReader::GetNumber<unsigned long long>(v_bp_workshop_id) : 0ull);

	BlueprintFolderReader::Storage.push_back(v_new_bp);
}

void BlueprintFolderReader::ReadBlueprintsFromFolder(const std::wstring& path)
{
	namespace fs = std::filesystem;

	std::error_code v_error_code;
	fs::directory_iterator v_dir_iter(path, fs::directory_options::skip_permission_denied, v_error_code);
	for (const auto& v_cur_item : v_dir_iter)
	{
		if (v_error_code) continue;

		if (v_cur_item.is_directory())
		{
			BlueprintFolderReader::ReadBlueprintFromFolder(v_cur_item.path().wstring());
		}
		else if (v_cur_item.is_regular_file())
		{
			//TODO: Make that work later
			DebugOutL("Found a possible blueprint file: ", v_cur_item.path().wstring());
		}
	}
}

void BlueprintFolderReader::ReadBlueprintsFromConfig()
{
	BlueprintFolderReader::ClearStorage();

	for (const std::wstring& v_bp_folder : DatabaseConfig::BlueprintFolders)
		BlueprintFolderReader::ReadBlueprintsFromFolder(v_bp_folder);

	DebugOutL("[BlueprintFolderReader] Successfully loaded ", BlueprintFolderReader::Storage.size(), " blueprints from ", DatabaseConfig::BlueprintFolders.size(), " folders");
}

void BlueprintFolderReader::ClearStorage()
{
	for (std::size_t a = 0; a < BlueprintFolderReader::Storage.size(); a++)
		delete BlueprintFolderReader::Storage[a];

	BlueprintFolderReader::Storage.clear();
	BlueprintFolderReader::SearchResults.clear();
}
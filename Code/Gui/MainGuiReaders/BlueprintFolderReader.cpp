#include "BlueprintFolderReader.hpp"

#include "ObjectDatabase\DatabaseConfig.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\Json.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

void BlueprintFolderReader::ReadBlueprintFromFile(const std::filesystem::path& path)
{
	if (!(path.has_stem() && path.has_root_directory()))
		return;

	BlueprintInstance* v_new_bp = new BlueprintInstance();
	v_new_bp->name = path.stem().wstring();
	v_new_bp->lower_name = String::ToLower(v_new_bp->name);
	v_new_bp->path = path.wstring();
	v_new_bp->workshop_id = 0ull;

	BlueprintFolderReader::Storage.push_back(v_new_bp);
}

void BlueprintFolderReader::ReadBlueprintFromFolder(const std::wstring& folder)
{
	const std::wstring v_description_path = folder + L"/description.json";
	if (!File::Exists(v_description_path))
		return;

	const std::wstring v_blueprint_path = folder + L"/blueprint.json";
	if (!File::Exists(v_blueprint_path))
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
	v_new_bp->path = v_blueprint_path;

	const std::wstring v_preview_img = folder + L"/icon.png";
	if (File::Exists(v_preview_img))
		v_new_bp->preview_image = v_preview_img;

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
			const fs::path& v_bp_path = v_cur_item.path();

			if (!(v_bp_path.has_extension() && v_bp_path.extension().string() == ".blueprint"))
				continue;

			BlueprintFolderReader::ReadBlueprintFromFile(v_bp_path);
		}
	}
}

void BlueprintFolderReader::ReadBlueprintsFromConfig()
{
	BlueprintFolderReader::ClearStorage();

	for (const auto& v_bp_folder : DatabaseConfig::BlueprintFolders)
		BlueprintFolderReader::ReadBlueprintsFromFolder(v_bp_folder.first);

	DebugOutL("[BlueprintFolderReader] Successfully loaded ", BlueprintFolderReader::Storage.size(), " blueprints from ", DatabaseConfig::BlueprintFolders.size(), " folders");
}

void BlueprintFolderReader::ClearStorage()
{
	for (std::size_t a = 0; a < BlueprintFolderReader::Storage.size(); a++)
		delete BlueprintFolderReader::Storage[a];

	BlueprintFolderReader::Storage.clear();
	BlueprintFolderReader::SearchResults.clear();
}
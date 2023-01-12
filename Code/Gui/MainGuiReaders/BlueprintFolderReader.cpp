#include "BlueprintFolderReader.hpp"
#pragma unmanaged

#include "ObjectDatabase\DatabaseConfig.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\Json.hpp"
#include "Utils\File.hpp"

#include <filesystem>

BlueprintInstance::BlueprintInstance(const std::wstring& name, const std::wstring& path, const unsigned long long& workshop_id)
{
	this->name = name;
	this->lower_name = String::ToLower(name);
	this->path = path;
	this->workshop_id = workshop_id;
}

bool BlueprintFolderReader::IsValidBlueprintFolder(const std::wstring& folder, std::wstring& v_name, std::wstring& v_path, unsigned long long& v_workshop_id)
{
	const std::wstring v_description_path = folder + L"/description.json";
	if (!File::Exists(v_description_path))
		return false;

	simdjson::dom::document v_bp_doc;
	if (!JsonReader::LoadParseSimdjsonCommentsC(v_description_path, v_bp_doc, simdjson::dom::element_type::OBJECT))
		return false;

	const auto v_root = v_bp_doc.root();
	const auto v_bp_type = v_root["type"];
	if (!(v_bp_type.is_string() && strcmp(v_bp_type.get_c_str(), "Blueprint") == 0))
		return false;

	const auto v_bp_name = v_root["name"];
	const auto v_bp_uuid = v_root["localId"];
	if (!(v_bp_name.is_string() && v_bp_uuid.is_string()))
		return false;

	v_name = String::ToWide(v_bp_name.get_string());
	v_path = folder;

	const auto v_bp_workshop_id = v_root["fileId"];
	v_workshop_id = (v_bp_workshop_id.is_number() ? JsonReader::GetNumber<unsigned long long>(v_bp_workshop_id) : 0ull);

	return true;
}

void BlueprintFolderReader::ReadBlueprintsFromFolder(const std::wstring& path)
{
	namespace fs = std::filesystem;

	std::error_code v_error_code;
	fs::directory_iterator v_dir_iter(path, fs::directory_options::skip_permission_denied, v_error_code);
	for (const auto& v_dir : v_dir_iter)
	{
		if (v_error_code || !v_dir.is_directory()) continue;

		std::wstring v_bp_path;
		std::wstring v_bp_name;
		unsigned long long v_bp_workshop_id;
		const fs::path& v_path = v_dir.path();
		if (!BlueprintFolderReader::IsValidBlueprintFolder(v_path.wstring(), v_bp_name, v_bp_path, v_bp_workshop_id))
			continue;

		BlueprintInstance* v_bp_instance = new BlueprintInstance(v_bp_name, v_bp_path, v_bp_workshop_id);
		BlueprintFolderReader::Storage.push_back(v_bp_instance);
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
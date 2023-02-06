#include "BlueprintFolderReader.hpp"

#include "ObjectDatabase\UserDataReaders\ItemModCounter.hpp"
#include "ObjectDatabase\DatabaseConfig.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\Json.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

bool BlueprintFolderReader::ShouldUseFilteredStorage()
{
	return (FilterSettingsData::UserDataFilter != UserDataFilter_Any);
}

std::vector<BlueprintInstance*>& BlueprintFolderReader::GetCurrentStorage()
{
	return BlueprintFolderReader::ShouldUseFilteredStorage()
		? BlueprintFolderReader::FilteredStorage
		: BlueprintFolderReader::Storage;
}

void BlueprintFolderReader::FilterStorage()
{
	BlueprintFolderReader::FilteredStorage.clear();

	for (BlueprintInstance* v_bp_instance : BlueprintFolderReader::Storage)
		if (v_bp_instance->v_filter & FilterSettingsData::UserDataFilter)
			BlueprintFolderReader::FilteredStorage.push_back(v_bp_instance);
}

void BlueprintFolderReader::LoadFromFile(const std::filesystem::path& path)
{
	if (!(path.has_stem() && path.has_parent_path()))
		return;

	BlueprintInstance* v_new_bp = new BlueprintInstance();
	v_new_bp->name = path.stem().wstring();
	v_new_bp->lower_name = String::ToLower(v_new_bp->name);
	v_new_bp->path = path.wstring();
	v_new_bp->directory = path.parent_path().wstring();
	v_new_bp->workshop_id = 0ull;
	v_new_bp->v_filter = FilterSettingsData::GetUserDataFilter(v_new_bp->path);

	BlueprintFolderReader::Storage.push_back(v_new_bp);
}

void BlueprintFolderReader::LoadFromFolder(const std::wstring& folder, const simdjson::dom::element& v_cur_elem)
{
	const std::wstring v_blueprint_path = folder + L"/blueprint.json";
	if (!File::Exists(v_blueprint_path))
		return;

	const auto v_bp_name = v_cur_elem["name"];
	const auto v_bp_uuid = v_cur_elem["localId"];
	if (!(v_bp_name.is_string() && v_bp_uuid.is_string()))
		return;

	BlueprintInstance* v_new_bp = new BlueprintInstance();
	v_new_bp->name = String::ToWide(v_bp_name.get_string());
	v_new_bp->lower_name = String::ToLower(v_new_bp->name);
	v_new_bp->path = v_blueprint_path;
	v_new_bp->directory = folder;
	v_new_bp->uuid = v_bp_uuid.get_c_str().value_unsafe();

	v_new_bp->v_filter = FilterSettingsData::GetUserDataFilter(v_new_bp->path);

	const std::wstring v_preview_img = folder + L"/icon.png";
	if (File::Exists(v_preview_img))
		v_new_bp->preview_image = v_preview_img;

	const auto v_bp_workshop_id = v_cur_elem["fileId"];
	v_new_bp->workshop_id = (v_bp_workshop_id.is_number() ? JsonReader::GetNumber<unsigned long long>(v_bp_workshop_id) : 0ull);

	BlueprintFolderReader::Storage.push_back(v_new_bp);
}

void BlueprintFolderReader::ClearStorage()
{
	for (std::size_t a = 0; a < BlueprintFolderReader::Storage.size(); a++)
		delete BlueprintFolderReader::Storage[a];

	BlueprintFolderReader::Storage.clear();
	BlueprintFolderReader::SearchResults.clear();
	BlueprintFolderReader::FilteredStorage.clear();
}
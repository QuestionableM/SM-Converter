#include "WorldFolderReader.hpp"

#include "ObjectDatabase\UserDataReaders\FilterSettingsData.hpp"

#include "Utils\File.hpp"

#pragma unmanaged

bool WorldFolderReader::ShouldUseFilteredStorage()
{
	return (FilterSettingsData::UserDataFilter != UserDataFilter_Any);
}

std::vector<WorldInstance*>& WorldFolderReader::GetCurrentStorage()
{
	return WorldFolderReader::ShouldUseFilteredStorage()
		? WorldFolderReader::FilteredStorage
		: WorldFolderReader::Storage;
}

void WorldFolderReader::FilterStorage()
{
	WorldFolderReader::FilteredStorage.clear();

	for (WorldInstance* v_world_instance : WorldFolderReader::Storage)
		if ((v_world_instance->v_filter & FilterSettingsData::UserDataFilter) != 0)
			WorldFolderReader::FilteredStorage.push_back(v_world_instance);
}

void WorldFolderReader::LoadFromFile(const std::filesystem::path& path)
{
	if (!(path.has_stem() && path.has_parent_path()))
		return;

	WorldInstance* v_new_world = new WorldInstance();
	v_new_world->name = path.stem().wstring();
	v_new_world->lower_name = String::ToLower(v_new_world->name);
	v_new_world->path = path.wstring();
	v_new_world->directory = path.parent_path().wstring();
	v_new_world->workshop_id = 0ull;
	v_new_world->v_filter = FilterSettingsData::GetUserDataFilter(v_new_world->path);

	WorldFolderReader::Storage.push_back(v_new_world);
}

void WorldFolderReader::LoadFromFolder(const std::wstring& path, const simdjson::dom::element& v_cur_elem)
{
	namespace fs = std::filesystem;

	const auto v_name = v_cur_elem["name"];
	const auto v_uuid = v_cur_elem["localId"];
	if (!(v_name.is_string() && v_uuid.is_string()))
		return;

	const std::wstring v_world_filename = String::ToWide(v_name.get_string());
	std::wstring v_world_path_str = path + L"/" + v_world_filename;

	const std::wstring v_wrld_path_world_ext = v_world_path_str + L".world";
	if (!File::Exists(v_wrld_path_world_ext))
	{
		const std::wstring v_wrld_path_json_ext = v_world_path_str + L".json";
		if (!File::Exists(v_wrld_path_json_ext))
			return;
		else
			v_world_path_str = v_wrld_path_json_ext;
	}
	else
	{
		v_world_path_str = v_wrld_path_world_ext;
	}

	WorldInstance* v_new_world = new WorldInstance();
	v_new_world->name = v_world_filename;
	v_new_world->lower_name = String::ToLower(v_new_world->name);
	v_new_world->uuid = v_uuid.get_c_str().value_unsafe();

	v_new_world->path = v_world_path_str;
	v_new_world->directory = path;

	const std::wstring v_preview_img = path + L"/" + v_new_world->name + L".png";
	if (File::Exists(v_preview_img))
		v_new_world->preview_image = v_preview_img;

	const auto v_workshop_id = v_cur_elem["fileId"];
	v_new_world->workshop_id = (v_workshop_id.is_number() ? JsonReader::GetNumber<unsigned long long>(v_workshop_id) : 0ull);

	v_new_world->v_filter = FilterSettingsData::GetUserDataFilter(v_new_world->path);

	WorldFolderReader::Storage.push_back(v_new_world);
}

void WorldFolderReader::ClearStorage()
{
	for (WorldInstance* v_world_instance : WorldFolderReader::Storage)
		delete v_world_instance;

	WorldFolderReader::Storage.clear();
	WorldFolderReader::SearchResults.clear();
	WorldFolderReader::FilteredStorage.clear();
}
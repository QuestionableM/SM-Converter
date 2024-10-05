#include "WorldFolderReader.hpp"

#include "ObjectDatabase\UserDataReaders\FilterSettingsData.hpp"

#include "Utils\File.hpp"

#pragma unmanaged

WorldInstance::WorldInstance(
	const SMUuid& wld_uuid,
	const std::wstring_view& wld_name,
	const std::wstring_view& wld_path,
	const std::wstring_view& wld_directory,
	const std::wstring_view& wld_image,
	std::uint64_t wld_workshop_id
) :
	uuid(wld_uuid),
	name(wld_name),
	lower_name(wld_name),
	path(wld_path),
	directory(wld_directory),
	preview_image(wld_image),
	workshop_id(wld_workshop_id),
	filter(FilterSettingsData::GetUserDataFilter(path))
{
	String::ToLower(lower_name);
}

/////////////// WORLD FOLDER READER ////////////////

bool WorldFolderReader::ShouldUseFilteredStorage()
{
	return (FilterSettingsData::UserDataFilter != UserDataFilter_Any);
}

void WorldFolderReader::FilterStorage()
{
	WorldFolderReader::FilteredStorage.clear();

	for (WorldInstance* v_world_instance : WorldFolderReader::Storage)
		if ((v_world_instance->filter & FilterSettingsData::UserDataFilter) != 0)
			WorldFolderReader::FilteredStorage.push_back(v_world_instance);
}

void WorldFolderReader::LoadFromFile(const std::filesystem::path& path)
{
	if (!(path.has_stem() && path.has_parent_path())) return;

	WorldFolderReader::PushToStorage(
		new WorldInstance(
			SMUuid::Null,
			path.stem().wstring(),
			path.wstring(),
			path.parent_path().wstring(),
			L"",
			0ULL
		)
	);
}

void WorldFolderReader::LoadFromFolder(const std::wstring& path, const simdjson::dom::element& v_cur_elem)
{
	namespace fs = std::filesystem;

	const auto v_name = v_cur_elem["name"];
	const auto v_uuid = v_cur_elem["localId"];
	if (!(v_name.is_string() && v_uuid.is_string()))
		return;

	std::wstring v_worldFilename = String::ToWide(v_name.get_string().value_unsafe());
	std::wstring v_worldPathStr = path + L"/" + v_worldFilename;

	const std::wstring v_wldPathWorldExt = v_worldPathStr + L".world";
	if (!File::Exists(v_wldPathWorldExt))
	{
		const std::wstring v_wldPathJsonExt = v_worldPathStr + L".json";
		if (!File::Exists(v_wldPathJsonExt)) return;

		v_worldPathStr = v_wldPathJsonExt;
	}
	else
	{
		v_worldPathStr = v_wldPathWorldExt;
	}

	const std::wstring v_previewImg = path + L"/" + v_worldFilename + L".png";
	const auto v_workshopId = v_cur_elem["fileId"];
	
	WorldFolderReader::PushToStorage(
		new WorldInstance(
			v_uuid.get_string().value_unsafe(),
			v_worldFilename,
			v_worldPathStr,
			path,
			File::Exists(v_previewImg) ? v_previewImg : L"",
			v_workshopId.is_number() ? JsonReader::GetNumber<std::uint64_t>(v_workshopId.value_unsafe()) : 0ULL
		)
	);
}

void WorldFolderReader::ClearStorage()
{
	for (WorldInstance* v_world_instance : WorldFolderReader::Storage)
		delete v_world_instance;

	WorldFolderReader::ClearBase();
}
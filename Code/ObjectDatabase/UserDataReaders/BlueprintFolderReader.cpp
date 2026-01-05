#include "BlueprintFolderReader.hpp"

#include "ObjectDatabase/UserDataReaders/ItemModCounter.hpp"
#include "ObjectDatabase/DatabaseConfig.hpp"

#include "Utils/Console.hpp"
#include "Utils/String.hpp"
#include "Utils/Json.hpp"
#include "Utils/File.hpp"

SM_UNMANAGED_CODE

BlueprintInstance::BlueprintInstance(
	SMUuid bp_uuid,
	const std::wstring_view& bp_name,
	const std::wstring_view& bp_path,
	const std::wstring_view& bp_directory,
	const std::wstring_view& bp_image,
	std::uint64_t bp_workshop_id
) :
	uuid(bp_uuid),
	name(bp_name),
	lower_name(bp_name),
	path(bp_path),
	directory(bp_directory),
	preview_image(bp_image),
	workshop_id(bp_workshop_id),
	filter(FilterSettingsData::GetUserDataFilter(path))
{
	String::ToLowerR(lower_name);
}

BlueprintInstance::BlueprintInstance(
	const std::wstring_view& bp_name,
	const std::wstring_view& bp_path,
	const std::wstring_view& bp_directory,
	std::uint64_t bp_workshop_id
) :
	BlueprintInstance(
		SMUuid::Null,
		bp_name,
		bp_path,
		bp_directory,
		L"",
		bp_workshop_id
	)
{}

//////////////// BLUEPRINT FOLDER READER //////////////

bool BlueprintFolderReader::ShouldUseFilteredStorage()
{
	return (FilterSettingsData::UserDataFilter != UserDataFilter_Any);
}

void BlueprintFolderReader::FilterStorage()
{
	BlueprintFolderReader::FilteredStorage.clear();

	for (BlueprintInstance* v_bp_instance : BlueprintFolderReader::Storage)
		if (v_bp_instance->filter & FilterSettingsData::UserDataFilter)
			BlueprintFolderReader::FilteredStorage.push_back(v_bp_instance);
}

void BlueprintFolderReader::LoadFromFile(const std::filesystem::path& path)
{
	if (!(path.has_stem() && path.has_parent_path()))
		return;

	BlueprintFolderReader::PushToStorage(
		new BlueprintInstance(
			path.stem().wstring(),
			path.wstring(),
			path.parent_path().wstring(),
			0ULL
		)
	);
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

	const std::wstring v_preview_img = folder + L"/icon.png";
	const auto v_workshopId = v_cur_elem["fileId"];

	BlueprintFolderReader::PushToStorage(
		new BlueprintInstance(
			v_bp_uuid.get_string().value_unsafe(),
			String::ToWide(v_bp_name.get_string().value_unsafe()),
			v_blueprint_path,
			folder,
			File::Exists(v_preview_img) ? v_preview_img : L"",
			v_workshopId.is_number() ? JsonReader::GetNumber<std::uint64_t>(v_workshopId.value_unsafe()) : 0ULL
		)
	);
}

void BlueprintFolderReader::ClearStorage()
{
	for (std::size_t a = 0; a < BlueprintFolderReader::Storage.size(); a++)
		delete BlueprintFolderReader::Storage[a];

	BlueprintFolderReader::ClearBase();
}
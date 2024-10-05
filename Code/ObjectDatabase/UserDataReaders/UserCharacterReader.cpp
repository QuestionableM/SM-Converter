#include "UserCharacterReader.hpp" 

#include "ObjectDatabase\UserDataReaders\FilterSettingsData.hpp"

#include "Utils\Console.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

UserCharacterInstance::UserCharacterInstance(
	const std::wstring_view& char_name,
	std::wstring&& char_path,
	const std::wstring_view& char_directory,
	const UserCharacterData& char_data
) :
	name(char_name),
	lower_name(char_name),
	path(std::move(char_path)),
	directory(char_directory),
	filter(FilterSettingsData::GetUserDataFilter(path)),
	character_data(char_data)
{}

////////////// USER CHARACTER READER ///////////////

bool UserCharacterReader::ShouldUseFilteredStorage()
{
	return (FilterSettingsData::UserDataFilter != UserDataFilter_Any);
}

void UserCharacterReader::FilterStorage()
{
	UserCharacterReader::FilteredStorage.clear();

	for (UserCharacterInstance* v_char_instance : UserCharacterReader::Storage)
		if ((v_char_instance->filter & FilterSettingsData::UserDataFilter) != 0)
			UserCharacterReader::FilteredStorage.push_back(v_char_instance);
}

void UserCharacterReader::LoadFromFile(const std::filesystem::path& path)
{
	if (!path.has_filename()) return;

	std::wstring v_char_file = path.wstring() + L"\\character";

	UserCharacterData v_char_data{};
	if (!v_char_data.from_file(v_char_file)) return;

	UserCharacterReader::PushToStorage(
		new UserCharacterInstance(
			path.filename().wstring(),
			std::move(v_char_file),
			path.wstring(),
			v_char_data
		)
	);
}

void UserCharacterReader::LoadCharacters()
{
	namespace fs = std::filesystem;

	std::wstring v_appdata_path;
	if (!File::GetAppDataPath(v_appdata_path))
		return;

	const std::wstring v_users_folder = v_appdata_path + L"\\Axolot Games\\Scrap Mechanic\\User";
	if (!File::Exists(v_users_folder))
		return;

	std::error_code v_error_code;
	fs::directory_iterator v_dir_iter(v_users_folder, fs::directory_options::skip_permission_denied, v_error_code);
	for (const auto& v_item : v_dir_iter)
	{
		if (!v_item.is_directory() || v_error_code) continue;
		UserCharacterReader::LoadFromFile(v_item.path());
	}
}

void UserCharacterReader::ClearStorage()
{
	for (UserCharacterInstance* v_char_instance : UserCharacterReader::Storage)
		delete v_char_instance;

	UserCharacterReader::ClearBase();
}
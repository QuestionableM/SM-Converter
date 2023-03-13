#include "UserCharacterReader.hpp" 

#include "ObjectDatabase\UserDataReaders\FilterSettingsData.hpp"

#include "Utils\Console.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

bool UserCharacterReader::ShouldUseFilteredStorage()
{
	return (FilterSettingsData::UserDataFilter != UserDataFilter_Any);
}

std::vector<UserCharacterInstance*>& UserCharacterReader::GetCurrentStorage()
{
	return UserCharacterReader::ShouldUseFilteredStorage()
		? UserCharacterReader::FilteredStorage
		: UserCharacterReader::Storage;
}

void UserCharacterReader::FilterStorage()
{
	UserCharacterReader::FilteredStorage.clear();

	for (UserCharacterInstance* v_char_instance : UserCharacterReader::Storage)
		if ((v_char_instance->v_filter & FilterSettingsData::UserDataFilter) != 0)
			UserCharacterReader::FilteredStorage.push_back(v_char_instance);
}

void UserCharacterReader::LoadFromFile(const std::filesystem::path& path)
{
	const std::wstring v_char_file = path.wstring() + L"\\character";

	if (!File::Exists(v_char_file)) return;
	if (!File::IsRegularFile(v_char_file)) return;

	DebugOutL("Character file: ", v_char_file);
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

	UserCharacterReader::Storage.clear();
	UserCharacterReader::SearchResults.clear();
	UserCharacterReader::FilteredStorage.clear();
}
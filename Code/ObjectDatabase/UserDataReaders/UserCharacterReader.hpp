#pragma once

#include "UStd/UnmanagedFilesystem.hpp"
#include "UStd/UnmanagedVector.hpp"

#include "UserCharacterData.hpp"
#include "UserDataBase.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Uuid.hpp"

SM_UNMANAGED_CODE

struct UserCharacterInstance
{
	UserCharacterInstance(
		const std::wstring_view& char_name,
		std::wstring&& char_path,
		const std::wstring_view& char_directory,
		const UserCharacterData& char_data
	);

	UserCharacterInstance(const UserCharacterInstance&) = delete;
	UserCharacterInstance(UserCharacterInstance&&) = delete;
	~UserCharacterInstance() = default;

	std::wstring name;
	std::wstring lower_name;

	std::wstring path;
	std::wstring directory;

	std::uint8_t filter;
	UserCharacterData character_data;
};

class UserCharacterReader
	: public UserDataBase<UserCharacterReader, UserCharacterInstance*>
{
public:
	static bool ShouldUseFilteredStorage();
	static void FilterStorage();
	
	static void LoadFromFile(const std::filesystem::path& path);
	static void LoadCharacters();

	static void ClearStorage();

private:
	UserCharacterReader() = default;
	UserCharacterReader(const UserCharacterReader&) = delete;
	UserCharacterReader(UserCharacterReader&&) = delete;
	~UserCharacterReader() = default;
};

SM_MANAGED_CODE
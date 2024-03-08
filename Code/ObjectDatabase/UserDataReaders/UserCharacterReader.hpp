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
	std::wstring name;
	std::wstring lower_name;
	
	std::wstring path;
	std::wstring directory;

	unsigned char v_filter;
	UserCharacterData character_data;

	UserCharacterInstance() = default;
	UserCharacterInstance(const UserCharacterInstance&) = delete;
	UserCharacterInstance(UserCharacterInstance&&) = delete;
	~UserCharacterInstance() = default;
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
#pragma once

#include "UStd\UnmanagedFilesystem.hpp"
#include "UStd\UnmanagedVector.hpp"

#include "UserCharacterData.hpp"

#include "Utils\Uuid.hpp"

#pragma unmanaged

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
{
public:
	using InstanceType = UserCharacterInstance;

	inline static std::vector<UserCharacterInstance*> Storage = {};
	inline static std::vector<UserCharacterInstance*> FilteredStorage = {};
	inline static std::vector<UserCharacterInstance*> SearchResults = {};

	static bool ShouldUseFilteredStorage();
	static std::vector<UserCharacterInstance*>& GetCurrentStorage();
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

#pragma managed
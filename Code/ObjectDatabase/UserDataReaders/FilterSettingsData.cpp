#include "FilterSettingsData.hpp"

#include "ObjectDatabase/DatabaseConfig.hpp"
#include "Utils/File.hpp"

SM_UNMANAGED_CODE

unsigned char FilterSettingsData::GetUserDataFilter(const std::wstring& path)
{
	if (File::IsSubPath(DatabaseConfig::GamePath, path))
		return UserDataFilter_GameItems;
	else if (File::IsSubPath(DatabaseConfig::WorkshopFolder, path))
		return UserDataFilter_WorkshopItems;
	else
		return UserDataFilter_UserItems;
}
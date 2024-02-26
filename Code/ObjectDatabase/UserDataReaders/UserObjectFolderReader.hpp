#pragma once

#include "UStd/UnmanagedString.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

class UserObjectFolderReader
{
	UserObjectFolderReader() = default;
	~UserObjectFolderReader() = default;

public:
	static void ReadItemFromDirectory(const std::wstring& path);

	static void LoadObjectsFromFolder(const std::wstring& folder);
	static void ReadFromConfig();
};

SM_MANAGED_CODE
#pragma once

#include "UStd\UnmanagedString.hpp"

#pragma unmanaged

class UserObjectFolderReader
{
	UserObjectFolderReader() = default;
	~UserObjectFolderReader() = default;

public:
	static void ReadItemFromDirectory(const std::wstring& path);

	static void LoadObjectsFromFolder(const std::wstring& folder);
	static void ReadFromConfig();
};

#pragma managed
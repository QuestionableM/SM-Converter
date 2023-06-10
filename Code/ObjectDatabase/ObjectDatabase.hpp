#pragma once

#include "ObjectDatabase\ObjectData.hpp"

#include "UStd\UnmanagedString.hpp"
#include "UStd\UnmanagedVector.hpp"

#include "Utils\Json.hpp"

#pragma unmanaged

class DatabaseLoader
{
	static void LoadGameDatabase();

	static void LoadModsFromPath(const std::wstring& path, bool is_local);
	static void LoadModsFromPaths(const std::vector<std::wstring>& path_vector, bool is_local);
	static void LoadModDatabase();

public:
	static void LoadDatabase();
	static void InitializeDatabase();
};

#pragma managed
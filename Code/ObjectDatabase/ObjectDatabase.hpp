#pragma once

#include "ObjectDatabase\ObjectData.hpp"

#include "Utils\UnmanagedString.hpp"
#include "Utils\UnmanagedVector.hpp"
#include "Utils\Json.hpp"

#pragma unmanaged

class DatabaseLoader
{
	static void LoadGameDatabase();

	static void LoadModsFromPaths(const std::vector<std::wstring>& path_vector, const bool& is_local);
	static void LoadModDatabase();

public:
	static void LoadDatabase();
	static void InitializeDatabase();
};

#pragma managed
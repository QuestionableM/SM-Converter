#pragma once

#include "ObjectDatabase\ObjectData.hpp"
#include "Utils\Json.hpp"

#include <vector>
#include <string>

class DatabaseLoader
{
	static void LoadGameDatabase();

	static void LoadModsFromPaths(const std::vector<std::wstring>& path_vector, const bool& is_local);
	static void LoadModDatabase();

public:
	static void LoadDatabase();
	static void InitializeDatabase();
};
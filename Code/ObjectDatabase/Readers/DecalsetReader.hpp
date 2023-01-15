#pragma once

#include "ObjectDatabase\Mods\Mod.hpp"

#include <string>

#pragma unmanaged

class DecalsetReader
{
public:
	static void LoadFromFile(const std::wstring& path, Mod* mod);
};

#pragma managed
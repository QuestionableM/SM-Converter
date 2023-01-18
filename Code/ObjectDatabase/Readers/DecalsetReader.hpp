#pragma once

#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\UnmanagedString.hpp"

#pragma unmanaged

class DecalsetReader
{
public:
	static void LoadFromFile(const std::wstring& path, Mod* mod);
};

#pragma managed
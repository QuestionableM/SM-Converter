#pragma once

#include "ObjectDatabase\Mods\Mod.hpp"

#include "UStd\UnmanagedString.hpp"

#pragma unmanaged

class DecalsetReader
{
public:
	static void LoadFromFile(const std::wstring& path, Mod* mod);
};

#pragma managed
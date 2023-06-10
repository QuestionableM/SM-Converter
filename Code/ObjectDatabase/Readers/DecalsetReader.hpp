#pragma once

#include "ObjectDatabase\Mods\Mod.hpp"

#include "UStd\UnmanagedString.hpp"

#pragma unmanaged

class DecalsetReader
{
	DecalsetReader() = default;

public:
	static void LoadFromFile(const std::wstring& path, SMMod* mod, bool add_to_global_db);
};

#pragma managed
#pragma once

#include "ObjectDatabase/Mods/Mod.hpp"

#include "UStd/UnmanagedString.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

class DecalsetReader
{
	DecalsetReader() = default;

public:
	static void LoadFromFile(const std::wstring& path, SMMod* mod, bool add_to_global_db);
};

SM_MANAGED_CODE
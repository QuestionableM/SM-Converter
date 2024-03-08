#pragma once

#include "ObjectDatabase/Mods/CustomGameMod.hpp"

#include "Converter/ConvertSettings.hpp"
#include "Converter/ConvertError.hpp"

#include "UStd/UnmanagedString.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

class WorldConverter
{
	static void WriteToFile(class SMWorld* v_world, const std::wstring& world_name, ConvertError& v_error);

public:
	static void ConvertToModel(const std::wstring& path, const std::wstring& name, ConvertError& v_error, CustomGame* v_custom_game);
};

SM_MANAGED_CODE
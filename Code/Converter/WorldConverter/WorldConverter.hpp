#pragma once

#include "ObjectDatabase/Mods/CustomGameMod.hpp"

#include "Converter/ConvertSettings.hpp"
#include "Converter/ConvertError.hpp"

#include "UStd/UnmanagedString.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

class WorldConverter
{
	static void WriteToFile(
		class SMWorld* pWorld,
		const std::wstring& world_name,
		ConvertError& error);

public:
	static void ConvertToModel(
		const std::wstring& path,
		const std::wstring& name,
		ConvertError& error,
		CustomGame* pCustomGame);
};

SM_MANAGED_CODE
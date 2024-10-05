#pragma once

#include "ObjectDatabase/Mods/CustomGameMod.hpp"

#include "Converter/ConvertSettings.hpp"
#include "Converter/ConvertError.hpp"

#include "UStd/UnmanagedString.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

class TileConv
{
	TileConv()  = default;
	~TileConv() = default;

	static void WriteToFileInternal(
		const class Tile& tile,
		const std::wstring& tile_name,
		ConvertError& error);

public:
	static void ConvertToModel(
		const std::wstring& tile_path,
		const std::wstring& tile_name,
		ConvertError& error,
		CustomGame* pCustomGame);
};

SM_MANAGED_CODE
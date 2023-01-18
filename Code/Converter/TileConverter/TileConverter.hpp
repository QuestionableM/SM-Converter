#pragma once

#include "Converter\ConvertSettings.hpp"
#include "Converter\ConvertError.hpp"

#include "Utils\UnmanagedString.hpp"

#pragma unmanaged

class TileConv
{
	TileConv()  = default;
	~TileConv() = default;

	static void WriteToFileInternal(class Tile* pTile, const std::wstring& tile_name, ConvertError& cError);
public:
	static void ConvertToModel(const std::wstring& tile_path, const std::wstring& tile_name, ConvertError& cError);
};

#pragma managed
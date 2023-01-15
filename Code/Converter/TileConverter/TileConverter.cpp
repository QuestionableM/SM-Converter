#include "TileConverter.hpp"

#include "Readers\TileReader.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

constexpr const static std::wstring_view g_TileOutputDirectory = L"./ConvertedTiles";

void TileConv::WriteToFileInternal(Tile* pTile, const std::wstring& tile_name, ConvertError& cError)
{
	if (cError) return; //Error check

	if (!File::CreateDirectorySafe(g_TileOutputDirectory.data()))
	{
		cError = ConvertError(1, L"Couldn't create the main output directory");
		return;
	}

	const std::wstring tile_dir_path = std::wstring(g_TileOutputDirectory.data()) + L"/" + tile_name;
	if (!File::CreateDirectorySafe(tile_dir_path))
	{
		cError = ConvertError(1, L"Coudln't create the tile output directory");
		return;
	}

	pTile->WriteToFile(tile_dir_path + L"/", tile_name);
}

void TileConv::ConvertToModel(const std::wstring& tile_path, const std::wstring& tile_name, ConvertError& cError)
{
	if (!File::IsRegularFile(tile_path))
	{
		cError = ConvertError(1, L"The specified path leads to a directory");
		return;
	}

	{
		Tile* pOutTile = TileReader::ReadTile(tile_path, cError);

		TileConv::WriteToFileInternal(pOutTile, tile_name, cError);

		delete pOutTile; //clear the tile data

		ModelStorage::ClearStorage(); // clear the cache
	}
}
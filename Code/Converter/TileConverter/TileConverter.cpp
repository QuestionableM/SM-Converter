#include "TileConverter.hpp"

#include "ObjectDatabase\DatabaseConfig.hpp"

#include "Readers\TileReader.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

void TileConv::WriteToFileInternal(Tile* pTile, const std::wstring& tile_name, ConvertError& cError)
{
	if (cError) return; //Error check

	const std::wstring v_tile_out_dir = std::wstring(DatabaseConfig::TileOutputFolder.data());
	if (!File::CreateDirectorySafe(v_tile_out_dir))
	{
		cError = ConvertError(1, L"Couldn't create the main output directory");
		return;
	}

	const std::wstring tile_dir_path = v_tile_out_dir + L"/" + tile_name;
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
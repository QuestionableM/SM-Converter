#include "TileConverter.hpp"

#include "ObjectDatabase\Mods\CustomGameSwitch.hpp"
#include "ObjectDatabase\DatabaseConfig.hpp"

#include "Readers\TileReader.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

void TileConv::WriteToFileInternal(const Tile& tile, const std::wstring& tile_name, ConvertError& error)
{
	if (error) return; //Error check

	const std::wstring v_tileOutDir(DatabaseConfig::TileOutputFolder);
	if (!File::CreateDirectorySafe(v_tileOutDir))
	{
		error.setError(1, "Couldn't create the main output directory");
		return;
	}

	const std::wstring v_tileDirPath = v_tileOutDir + L"/" + tile_name;
	if (!File::CreateDirectorySafe(v_tileDirPath))
	{
		error.setError(1, "Coudln't create the tile output directory");
		return;
	}

	tile.WriteToFile(v_tileDirPath + L"/", tile_name);
}

void TileConv::ConvertToModel(
	const std::wstring& tile_path,
	const std::wstring& tile_name,
	ConvertError& error,
	CustomGame* pCustomGame)
{
	if (!File::IsRegularFile(tile_path))
	{
		error.setError(1, "The specified path leads to a directory");
		return;
	}

	Tile v_outputTile;
	if (pCustomGame)
	{
		//The original content will be set back automatically by the SMModCustomGameSwitch destructor
		SMModCustomGameSwitch<false, true> v_content_switch;
		v_content_switch.MergeContent(pCustomGame);

		TileReader::ReadTile<false>(tile_path, error, v_outputTile);
	}
	else
	{
		TileReader::ReadTile<false>(tile_path, error, v_outputTile);
	}

	TileConv::WriteToFileInternal(v_outputTile, tile_name, error);
	ModelStorage::ClearStorage();
}
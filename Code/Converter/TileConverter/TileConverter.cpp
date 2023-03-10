#include "TileConverter.hpp"

#include "ObjectDatabase\Mods\CustomGameSwitch.hpp"
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

void TileConv::ConvertToModel(const std::wstring& tile_path, const std::wstring& tile_name, ConvertError& cError, CustomGame* v_custom_game)
{
	if (!File::IsRegularFile(tile_path))
	{
		cError = ConvertError(1, L"The specified path leads to a directory");
		return;
	}

	Tile* v_output_tile = nullptr;
	if (v_custom_game)
	{
		//The original content will be set back automatically by the SMModCustomGameSwitch destructor
		SMModCustomGameSwitch<false, true> v_content_switch;
		v_content_switch.MergeContent(v_custom_game);

		v_output_tile = TileReader::ReadTile<false>(tile_path, cError);
	}
	else
	{
		v_output_tile = TileReader::ReadTile<false>(tile_path, cError);
	}

	TileConv::WriteToFileInternal(v_output_tile, tile_name, cError);

	delete v_output_tile;
	ModelStorage::ClearStorage();
}
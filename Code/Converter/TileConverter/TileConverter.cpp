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
		//Sets the path replacement for $CONTENT_DATA
		SMMod* v_custom_game_mod = v_custom_game;
		v_custom_game_mod->SetContentKey();

		//Copy the game data to restore it after the conversion
		const auto v_part_list_copy = SMMod::PartStorage;
		const auto v_block_list_copy = SMMod::BlockStorage;
		const auto v_hvs_list_copy = SMMod::HarvestableStorage;
		const auto v_asset_list_copy = SMMod::AssetStorage;
		if (v_custom_game->ShouldUseGameContent())
		{
			//Add to the existing storage if the custom game uses game content
			SMMod::MergeMaps(SMMod::PartStorage, v_custom_game_mod->m_Parts);
			SMMod::MergeMaps(SMMod::BlockStorage, v_custom_game_mod->m_Blocks);
			SMMod::MergeMaps(SMMod::HarvestableStorage, v_custom_game_mod->m_Harvestables);
			SMMod::MergeMaps(SMMod::AssetStorage, v_custom_game_mod->m_Assets);
		}
		else
		{
			//Overwrite the global list if the custom game doesn't use game content
			SMMod::PartStorage = v_custom_game_mod->m_Parts;
			SMMod::BlockStorage = v_custom_game_mod->m_Blocks;
			SMMod::HarvestableStorage = v_custom_game_mod->m_Harvestables;
			SMMod::AssetStorage = v_custom_game_mod->m_Assets;
		}

		v_output_tile = TileReader::ReadTile<false>(tile_path, cError);

		//Set the original content back
		SMMod::PartStorage = v_part_list_copy;
		SMMod::BlockStorage = v_block_list_copy;
		SMMod::HarvestableStorage = v_hvs_list_copy;
		SMMod::AssetStorage = v_asset_list_copy;

		KeywordReplacer::ClearContentKey();
	}
	else
	{
		v_output_tile = TileReader::ReadTile<false>(tile_path, cError);
	}

	TileConv::WriteToFileInternal(v_output_tile, tile_name, cError);

	delete v_output_tile;
	ModelStorage::ClearStorage();
}
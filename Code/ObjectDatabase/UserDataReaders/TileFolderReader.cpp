#include "TileFolderReader.hpp"

#include "Converter\TileConverter\Readers\TileReader.hpp"
#include "Converter\TileConverter\TileHeader.hpp"

#include "ObjectDatabase\DatabaseConfig.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\Json.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

bool TileFolderReader::ShouldUseFilteredStorage()
{
	return (FilterSettingsData::TileSizeFilter != TileSizeFilter_Any || FilterSettingsData::UserDataFilter != UserDataFilter_Any);
}

void TileFolderReader::FilterStorage()
{
	TileFolderReader::FilteredStorage.clear();

	for (TileInstance* v_tile_instance : TileFolderReader::Storage)
		if ((v_tile_instance->v_filter & FilterSettingsData::UserDataFilter) != 0 &&
			(v_tile_instance->v_size_filter & FilterSettingsData::TileSizeFilter) != 0)
			TileFolderReader::FilteredStorage.push_back(v_tile_instance);
}

TileSizeFilter TileFolderReader::GetTileSize(int v_sz)
{
	switch (v_sz)
	{
	case 0: case 1:
		return TileSizeFilter_Small;
	case 2:
		return TileSizeFilter_Medium;
	case 3: case 4:
		return TileSizeFilter_Large;
	default:
		return TileSizeFilter_ExtraLarge;
	}
}

void TileFolderReader::GetTileData(const std::wstring& path, ConvertError& v_error)
{
	Tile* v_tile = TileReader::ReadTile<true>(path, v_error);
	if (v_tile) delete v_tile;
}

void TileFolderReader::InitializeTileKeys()
{
	for (TileInstance* v_tile_instance : TileFolderReader::Storage)
		if ((v_tile_instance->v_filter & UserDataFilter_GameItems) == 0)
			KeywordReplacer::CreateContentKey(v_tile_instance->uuid, v_tile_instance->directory);
}

void TileFolderReader::LoadFromFile(const std::filesystem::path& path)
{
	if (!(path.has_stem() && path.has_filename() && path.has_parent_path()))
		return;

	TileHeaderBaseInfo v_tile_info;
	ConvertError v_error;
	if (!TileReader::ReadTileHeader(path.wstring(), v_tile_info, v_error))
		return;

	if (TileFolderReader::TileMap.find(v_tile_info.uuid) != TileFolderReader::TileMap.end())
	{
		DebugWarningL("The tile with the specified key already exists! (Uuid: ", v_tile_info.uuid.ToString(), ")");
		return;
	}

	TileInstance* v_new_tile = new TileInstance();
	v_new_tile->name = path.stem().wstring();
	v_new_tile->lower_name = String::ToLower(v_new_tile->name);
	v_new_tile->uuid = v_tile_info.uuid;

	v_new_tile->path = path.wstring();
	v_new_tile->directory = path.parent_path().wstring();

	v_new_tile->creator_id = v_tile_info.creator_id;
	v_new_tile->workshop_id = 0ull;

	v_new_tile->v_size_filter = TileFolderReader::GetTileSize(v_tile_info.width);
	v_new_tile->v_filter = FilterSettingsData::GetUserDataFilter(v_new_tile->path);

	const std::wstring v_preview_img = path.parent_path().wstring() + L"/" + v_tile_info.uuid.ToWstring() + L".png";
	if (File::Exists(v_preview_img))
		v_new_tile->preview_image = v_preview_img;

	TileFolderReader::PushToStorage(v_new_tile);
	TileFolderReader::TileMap.insert(std::make_pair(v_new_tile->uuid, v_new_tile));
}

void TileFolderReader::LoadFromFolder(const std::wstring& path, const simdjson::dom::element& v_cur_elem)
{
	namespace fs = std::filesystem;

	const auto v_name = v_cur_elem["name"];
	const auto v_uuid = v_cur_elem["localId"];

	if (!(v_name.is_string() && v_uuid.is_string()))
		return;

	const std::wstring v_tile_filename = String::ToWide(v_name.get_string());
	const SMUuid v_content_uuid = v_uuid.get_c_str().value();

	std::wstring v_tile_path_str = path + L"/" + v_tile_filename;
	if (!File::Exists(v_tile_path_str))
	{
		v_tile_path_str.append(L".tile");
		if (!File::Exists(v_tile_path_str))
			return;
	}

	fs::path v_tile_path = v_tile_path_str;
	if (!v_tile_path.has_stem())
		return;

	TileHeaderBaseInfo v_tile_info;
	ConvertError v_error;
	if (!TileReader::ReadTileHeader(v_tile_path.wstring(), v_tile_info, v_error))
		return;

	if (TileFolderReader::TileMap.find(v_content_uuid) != TileFolderReader::TileMap.end())
	{
		DebugWarningL("The tile with the specified key already exists! (Uuid: ", v_content_uuid.ToString(), ")");
		return;
	}

	TileInstance* v_new_tile = new TileInstance();
	v_new_tile->name = v_tile_path.stem().wstring();
	v_new_tile->lower_name = String::ToLower(v_new_tile->name);
	v_new_tile->uuid = v_content_uuid;

	v_new_tile->path = v_tile_path.wstring();
	v_new_tile->directory = path;

	const std::wstring v_preview_img = path + L"/" + v_tile_info.uuid.ToWstring() + L".png";
	if (File::Exists(v_preview_img))
		v_new_tile->preview_image = v_preview_img;

	const auto v_workshop_id = v_cur_elem["fileId"];
	v_new_tile->workshop_id = (v_workshop_id.is_number() ? JsonReader::GetNumber<unsigned long long>(v_workshop_id) : 0ull);
	v_new_tile->creator_id = v_tile_info.creator_id;

	v_new_tile->v_size_filter = TileFolderReader::GetTileSize(v_tile_info.width);
	v_new_tile->v_filter = FilterSettingsData::GetUserDataFilter(v_new_tile->path);

	TileFolderReader::PushToStorage(v_new_tile);
	TileFolderReader::TileMap.insert(std::make_pair(v_new_tile->uuid, v_new_tile));
}

void TileFolderReader::ClearStorage()
{
	for (std::size_t a = 0; a < TileFolderReader::Storage.size(); a++)
		delete TileFolderReader::Storage[a];

	TileFolderReader::TileMap.clear();
	TileFolderReader::ClearBase();
}
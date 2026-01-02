#include "TileFolderReader.hpp"

#include "Converter/TileConverter/Readers/TileReader.hpp"
#include "Converter/TileConverter/TileHeader.hpp"

#include "ObjectDatabase/DatabaseConfig.hpp"

#include "Utils/Console.hpp"
#include "Utils/String.hpp"
#include "Utils/Json.hpp"
#include "Utils/File.hpp"

SM_UNMANAGED_CODE

TileInstance::TileInstance(
	const SMUuid& tile_uuid,
	const std::wstring_view& tile_name,
	const std::wstring_view& tile_path,
	const std::wstring_view& tile_directory,
	const std::wstring_view& tile_image,
	std::uint64_t tile_workshop_id,
	std::uint64_t tile_creator_id,
	std::uint8_t size
)
	: uuid(tile_uuid)
	, name(tile_name)
	, lower_name(tile_name)
	, path(tile_path)
	, directory(tile_directory)
	, preview_image(tile_image)
	, workshop_id(tile_workshop_id)
	, creator_id(tile_creator_id)
	, filter(FilterSettingsData::GetUserDataFilter(path))
	, size_filter(size)
{
	String::ToLowerR(lower_name);
}

//////////////// TILE FOLDER READER ///////////////

bool TileFolderReader::ShouldUseFilteredStorage()
{
	return (FilterSettingsData::TileSizeFilter != TileSizeFilter_Any || FilterSettingsData::UserDataFilter != UserDataFilter_Any);
}

void TileFolderReader::FilterStorage()
{
	TileFolderReader::FilteredStorage.clear();

	for (TileInstance* v_tile_instance : TileFolderReader::Storage)
		if ((v_tile_instance->filter & FilterSettingsData::UserDataFilter) != 0 &&
			(v_tile_instance->size_filter & FilterSettingsData::TileSizeFilter) != 0)
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
	Tile v_tileData;
	TileReader::ReadTile<true>(path, v_error, v_tileData);
}

void TileFolderReader::InitializeTileKeys()
{
	for (TileInstance* v_tile_instance : TileFolderReader::Storage)
		if ((v_tile_instance->filter & UserDataFilter_GameItems) == 0)
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
		DebugWarningL("The tile with the specified key already exists! (Uuid: ", v_tile_info.uuid.toString(), ")");
		return;
	}

	const std::wstring v_previewImg = path.parent_path().wstring() + L"/" + v_tile_info.uuid.toWstring() + L".png";

	TileInstance* v_pNewTile = new TileInstance(
		v_tile_info.uuid,
		path.stem().wstring(),
		path.wstring(),
		path.parent_path().wstring(),
		File::Exists(v_previewImg) ? v_previewImg : L"",
		0ULL,
		v_tile_info.creator_id,
		TileFolderReader::GetTileSize(v_tile_info.width)
	);

	TileFolderReader::PushToStorage(v_pNewTile);
	TileFolderReader::TileMap.emplace(v_pNewTile->uuid, v_pNewTile);
}

void TileFolderReader::LoadFromFolder(const std::wstring& path, const simdjson::dom::element& v_cur_elem)
{
	namespace fs = std::filesystem;

	const auto v_name = v_cur_elem["name"];
	const auto v_uuid = v_cur_elem["localId"];

	if (!(v_name.is_string() && v_uuid.is_string()))
		return;

	const std::wstring v_tile_filename = String::ToWide(v_name.get_string().value_unsafe());
	const SMUuid v_content_uuid = v_uuid.get_string().value_unsafe();

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
		DebugWarningL("The tile with the specified key already exists! (Uuid: ", v_content_uuid.toString(), ")");
		return;
	}

	const std::wstring v_previewImg = path + L"/" + v_tile_info.uuid.toWstring() + L".png";
	const auto v_workshopId = v_cur_elem["fileId"];

	TileInstance* v_pNewTile = new TileInstance(
		v_content_uuid,
		v_tile_path.stem().wstring(),
		v_tile_path.wstring(),
		path,
		File::Exists(v_previewImg) ? v_previewImg : L"",
		v_workshopId.is_number() ? JsonReader::GetNumber<std::uint64_t>(v_workshopId.value_unsafe()) : 0ULL,
		v_tile_info.creator_id,
		TileFolderReader::GetTileSize(v_tile_info.width)
	);

	TileFolderReader::PushToStorage(v_pNewTile);
	TileFolderReader::TileMap.emplace(v_pNewTile->uuid, v_pNewTile);
}

void TileFolderReader::ClearStorage()
{
	for (std::size_t a = 0; a < TileFolderReader::Storage.size(); a++)
		delete TileFolderReader::Storage[a];

	TileFolderReader::TileMap.clear();
	TileFolderReader::ClearBase();
}
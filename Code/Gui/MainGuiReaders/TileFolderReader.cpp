#include "TileFolderReader.hpp"

#include "ObjectDatabase\DatabaseConfig.hpp"
#include "Converter\TileConverter\TileHeader.hpp"
#include "Converter\TileConverter\Readers\TileReader.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\Json.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

void TileFolderReader::LoadFromFile(const std::filesystem::path& path)
{
	if (!(path.has_stem() && path.has_filename() && path.has_parent_path()))
		return;

	TileHeaderBaseInfo v_tile_info;
	ConvertError v_error;
	if (!TileReader::ReadTileHeader(path.wstring(), v_tile_info, v_error))
		return;

	TileInstance* v_new_tile = new TileInstance();
	v_new_tile->name = path.stem().wstring();
	v_new_tile->lower_name = String::ToLower(v_new_tile->name);

	v_new_tile->path = path.wstring();

	v_new_tile->creator_id = v_tile_info.creator_id;
	v_new_tile->workshop_id = 0ull;

	const std::wstring v_preview_img = path.parent_path().wstring() + L"/" + v_tile_info.uuid.ToWstring() + L".png";
	if (File::Exists(v_preview_img))
		v_new_tile->preview_image = v_preview_img;

	TileFolderReader::Storage.push_back(v_new_tile);
}

void TileFolderReader::LoadFromDirectory(const std::wstring& path)
{
	namespace fs = std::filesystem;

	const std::wstring v_desc_path = path + L"/description.json";
	if (!File::Exists(v_desc_path))
		return;

	simdjson::dom::document v_desc_doc;
	if (!JsonReader::LoadParseSimdjsonCommentsC(v_desc_path, v_desc_doc, simdjson::dom::element_type::OBJECT))
		return;

	const auto v_root = v_desc_doc.root();

	const auto v_type = v_root["type"];
	if (!(v_type.is_string() && strcmp(v_type.get_c_str(), "Tile") == 0))
		return;

	const auto v_uuid = v_root["localId"];
	const auto v_name = v_root["name"];
	if (!(v_uuid.is_string() && v_name.is_string()))
		return;

	const std::wstring v_tile_filename = String::ToWide(v_name.get_string());
	const fs::path v_tile_path = path + L"/" + v_tile_filename;
	if (!v_tile_path.has_stem())
		return;

	TileHeaderBaseInfo v_tile_info;
	ConvertError v_error;
	if (!TileReader::ReadTileHeader(v_tile_path.wstring(), v_tile_info, v_error))
		return;

	TileInstance* v_new_tile = new TileInstance();
	v_new_tile->name = v_tile_path.stem().wstring();
	v_new_tile->lower_name = String::ToLower(v_new_tile->name);
	v_new_tile->path = v_tile_path.wstring();

	const std::wstring v_preview_img = path + L"/" + v_tile_info.uuid.ToWstring() + L".png";
	if (File::Exists(v_preview_img))
		v_new_tile->preview_image = v_preview_img;

	const auto v_workshop_id = v_root["fileId"];
	v_new_tile->workshop_id = (v_workshop_id.is_number() ? JsonReader::GetNumber<unsigned long long>(v_workshop_id) : 0ull);
	v_new_tile->creator_id = v_tile_info.creator_id;

	TileFolderReader::Storage.push_back(v_new_tile);
}

void TileFolderReader::ReadTilesFromFolder(const std::wstring& path)
{
	namespace fs = std::filesystem;

	std::error_code v_error_code;
	fs::directory_iterator v_dir_iter(path, fs::directory_options::skip_permission_denied, v_error_code);
	for (const auto& v_cur_item : v_dir_iter)
	{
		if (v_error_code) continue;

		if (v_cur_item.is_regular_file())
		{
			const fs::path& v_file_path = v_cur_item.path();
			if (!(v_file_path.has_extension() && v_file_path.extension().string() == ".tile"))
				continue;

			TileFolderReader::LoadFromFile(v_file_path);
		}
		else if (v_cur_item.is_directory())
		{
			TileFolderReader::LoadFromDirectory(v_cur_item.path().wstring());
		}
	}
}

void TileFolderReader::ReadTilesFromConfig()
{
	TileFolderReader::ClearStorage();

	for (const auto& v_tile_folder : DatabaseConfig::TileFolders)
		TileFolderReader::ReadTilesFromFolder(v_tile_folder.first);

	DebugOutL("[TileFolderReader] Successfully loaded ", TileFolderReader::Storage.size(), " tiles from ", DatabaseConfig::TileFolders.size(), " folders");
}

void TileFolderReader::ClearStorage()
{
	for (std::size_t a = 0; a < TileFolderReader::Storage.size(); a++)
		delete TileFolderReader::Storage[a];

	TileFolderReader::Storage.clear();
	TileFolderReader::SearchResults.clear();
}
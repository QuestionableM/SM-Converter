#pragma once

#include "UStd/UnmanagedFilesystem.hpp"
#include "UStd/UnmanagedString.hpp"
#include "UStd/UnmanagedVector.hpp"

#include "Converter/ConvertError.hpp"
#include "FilterSettingsData.hpp"
#include "UserDataBase.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Uuid.hpp"
#include "Utils/Json.hpp"

SM_UNMANAGED_CODE

struct TileInstance
{
	TileInstance(
		const SMUuid& tile_uuid,
		const std::wstring_view& tile_name,
		const std::wstring_view& tile_path,
		const std::wstring_view& tile_directory,
		const std::wstring_view& tile_image,
		std::uint64_t tile_workshop_id,
		std::uint64_t tile_creator_id,
		std::uint8_t size
	);

	TileInstance(const TileInstance&) = delete;
	TileInstance(TileInstance&&) = delete;
	~TileInstance() = default;

	SMUuid uuid;

	std::wstring name;
	std::wstring lower_name;

	std::wstring path;
	std::wstring directory;
	std::wstring preview_image;

	std::uint64_t workshop_id;
	std::uint64_t creator_id;

	std::uint8_t filter;
	std::uint8_t size_filter;
};

class TileFolderReader :
	public UserDataBase<TileFolderReader, TileInstance*>
{
public:
	static bool ShouldUseFilteredStorage();
	static void FilterStorage();

	static TileSizeFilter GetTileSize(int v_sz);
	static void GetTileData(const std::wstring& path, ConvertError& v_error);
	//Creates `$CONTENT_<uuid>` keys for all non-vanilla tiles
	static void InitializeTileKeys();

	static void LoadFromFile(const std::filesystem::path& path);
	static void LoadFromFolder(const std::wstring& path, const simdjson::dom::element& v_cur_elem);

	static void ClearStorage();

private:
	inline static std::unordered_map<SMUuid, TileInstance*> TileMap = {};

	TileFolderReader() = default;
	TileFolderReader(const TileFolderReader&) = delete;
	TileFolderReader(TileFolderReader&&) = delete;
	~TileFolderReader() = default;
};

SM_MANAGED_CODE
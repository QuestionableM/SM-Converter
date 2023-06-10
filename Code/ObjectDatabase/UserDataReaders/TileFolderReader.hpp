#pragma once

#include "UStd\UnmanagedFilesystem.hpp"
#include "UStd\UnmanagedString.hpp"
#include "UStd\UnmanagedVector.hpp"

#include "Converter\ConvertError.hpp"
#include "FilterSettingsData.hpp"
#include "Utils\Uuid.hpp"
#include "Utils\Json.hpp"

#pragma unmanaged

struct TileInstance
{
	std::wstring name;
	std::wstring lower_name;
	SMUuid uuid;

	std::wstring path;
	std::wstring directory;

	std::wstring preview_image;

	unsigned long long workshop_id;
	unsigned long long creator_id;

	unsigned char v_filter;
	unsigned char v_size_filter;

	TileInstance() = default;
	TileInstance(const TileInstance&) = delete;
	TileInstance(TileInstance&&) = delete;
	~TileInstance() = default;
};

class TileFolderReader
{
public:
	using InstanceType = TileInstance;

	inline static std::vector<TileInstance*> Storage = {};
	inline static std::vector<TileInstance*> FilteredStorage = {};
	inline static std::vector<TileInstance*> SearchResults = {};

	static bool ShouldUseFilteredStorage();
	static std::vector<TileInstance*>& GetCurrentStorage();
	static void FilterStorage();

	static TileSizeFilter GetTileSize(int v_sz);
	static void GetTileData(TileInstance* v_tile_instance, ConvertError& v_error);
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

#pragma managed
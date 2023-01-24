#pragma once

#include "UStd\UnmanagedFilesystem.hpp"
#include "UStd\UnmanagedString.hpp"
#include "UStd\UnmanagedVector.hpp"

#include "FilterSettingsData.hpp"
#include "Utils\Uuid.hpp"

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
	TileInstance(TileInstance&) = delete;
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

	static TileSizeFilter GetTileSize(const int& v_sz);

	static void LoadFromFile(const std::filesystem::path& path);
	static void LoadFromDirectory(const std::wstring& path);

	static void ReadTilesFromFolder(const std::wstring& path);
	static void ReadTilesFromConfig();

	static void ClearStorage();

private:
	TileFolderReader() = default;
	TileFolderReader(const TileFolderReader&) = delete;
	TileFolderReader(TileFolderReader&) = delete;
	~TileFolderReader() = default;
};

#pragma managed
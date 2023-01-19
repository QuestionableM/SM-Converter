#pragma once

#include "UStd\UnmanagedFilesystem.hpp"
#include "UStd\UnmanagedString.hpp"
#include "UStd\UnmanagedVector.hpp"

#pragma unmanaged

struct TileInstance
{
	std::wstring name;
	std::wstring lower_name;

	std::wstring path;
	std::wstring directory;

	std::wstring preview_image;

	unsigned long long workshop_id;
	unsigned long long creator_id;

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
	inline static std::vector<TileInstance*> SearchResults = {};

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
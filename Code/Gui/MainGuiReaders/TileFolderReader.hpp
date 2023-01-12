#pragma once

#include <filesystem>
#include <vector>
#include <string>

struct TileInstance
{
	std::wstring name;
	std::wstring lower_name;

	std::wstring filename;
	std::wstring directory;

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
	inline static std::vector<TileInstance*> TileStorage = {};
	inline static std::vector<TileInstance*> TileSearchResults = {};

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
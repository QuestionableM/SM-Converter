#pragma once

#include "UStd\UnmanagedFilesystem.hpp"
#include "Ustd\UnmanagedString.hpp"
#include "Ustd\UnmanagedVector.hpp"

#pragma unmanaged

struct BlueprintInstance
{
	std::wstring name;
	//Used for more optimized search
	std::wstring lower_name;

	std::wstring path;
	std::wstring directory;

	std::wstring preview_image;

	unsigned long long workshop_id;

	BlueprintInstance() = default;
	BlueprintInstance(const BlueprintInstance&) = delete;
	BlueprintInstance(BlueprintInstance&) = delete;
	~BlueprintInstance() = default;
};

class BlueprintFolderReader
{
public:
	using InstanceType = BlueprintInstance;

	inline static std::vector<BlueprintInstance*> Storage = {};
	inline static std::vector<BlueprintInstance*> SearchResults = {};

	static void ReadBlueprintFromFile(const std::filesystem::path& path);
	static void ReadBlueprintFromFolder(const std::wstring& folder);
	static void ReadBlueprintsFromFolder(const std::wstring& path);
	static void ReadBlueprintsFromConfig();
	static void ClearStorage();

private:
	BlueprintFolderReader() = default;
	BlueprintFolderReader(const BlueprintFolderReader&) = delete;
	BlueprintFolderReader(BlueprintFolderReader&) = delete;
	~BlueprintFolderReader() = default;
};

#pragma managed
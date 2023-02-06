#pragma once

#include "UStd\UnmanagedFilesystem.hpp"
#include "Ustd\UnmanagedString.hpp"
#include "Ustd\UnmanagedVector.hpp"

#include "ObjectDatabase/Mods/Mod.hpp"
#include "FilterSettingsData.hpp"
#include "Utils\Uuid.hpp"
#include "Utils\Json.hpp"

#pragma unmanaged

struct BlueprintInstance
{
	std::wstring name;
	//Used for more optimized search
	std::wstring lower_name;
	SMUuid uuid;

	std::wstring path;
	std::wstring directory;
	std::wstring preview_image;

	unsigned long long workshop_id;
	unsigned char v_filter;

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
	inline static std::vector<BlueprintInstance*> FilteredStorage = {};
	inline static std::vector<BlueprintInstance*> SearchResults = {};

	static bool ShouldUseFilteredStorage();
	static std::vector<BlueprintInstance*>& GetCurrentStorage();
	static void FilterStorage();

	static void LoadFromFile(const std::filesystem::path& path);
	static void LoadFromFolder(const std::wstring& folder, const simdjson::dom::element& v_cur_elem);
	
	static void ClearStorage();

private:
	BlueprintFolderReader() = default;
	BlueprintFolderReader(const BlueprintFolderReader&) = delete;
	BlueprintFolderReader(BlueprintFolderReader&) = delete;
	~BlueprintFolderReader() = default;
};

#pragma managed
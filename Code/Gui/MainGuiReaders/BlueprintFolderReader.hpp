#pragma once
#pragma unmanaged

#include <vector>
#include <string>

struct BlueprintInstance
{
	std::wstring name;
	std::wstring path;

	unsigned long long workshop_id;
};

class BlueprintFolderReader
{
public:
	inline static std::vector<BlueprintInstance> BlueprintStorage = {};

	static bool IsValidBlueprintFolder(const std::wstring& folder, BlueprintInstance& v_instance);
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
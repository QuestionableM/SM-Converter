#pragma once
#pragma unmanaged

#include <vector>
#include <string>

struct BlueprintInstance
{
	std::wstring name;
	//Used for more optimized search
	std::wstring lower_name;

	std::wstring path;

	unsigned long long workshop_id;

	BlueprintInstance(const std::wstring& name, const std::wstring& path, const unsigned long long& workshop_id);
	BlueprintInstance(const BlueprintInstance&) = delete;
	BlueprintInstance(BlueprintInstance&) = delete;
	~BlueprintInstance() = default;
};

class BlueprintFolderReader
{
public:
	inline static std::vector<BlueprintInstance*> BlueprintStorage = {};
	inline static std::vector<BlueprintInstance*> BlueprintSearchResults = {};

	static bool IsValidBlueprintFolder(const std::wstring& folder, std::wstring& v_name, std::wstring& v_path, unsigned long long& v_workshop_id);
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
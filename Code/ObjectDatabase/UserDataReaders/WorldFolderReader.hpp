#pragma once

#include "UStd/UnmanagedFilesystem.hpp"
#include "UStd/UnmanagedString.hpp"
#include "UStd/UnmanagedVector.hpp"
#include "UserDataBase.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Uuid.hpp"
#include "Utils/Json.hpp"

SM_UNMANAGED_CODE

struct WorldInstance
{
	std::wstring name;
	std::wstring lower_name;
	SMUuid uuid;

	std::wstring path;
	std::wstring directory;
	std::wstring preview_image;

	unsigned long long workshop_id;
	unsigned char v_filter;

	WorldInstance() = default;
	WorldInstance(const WorldInstance&) = delete;
	WorldInstance(WorldInstance&) = delete;
	~WorldInstance() = default;
};

class WorldFolderReader :
	public UserDataBase<WorldFolderReader, WorldInstance*>
{
public:
	static bool ShouldUseFilteredStorage();
	static void FilterStorage();

	static void LoadFromFile(const std::filesystem::path& path);
	static void LoadFromFolder(const std::wstring& path, const simdjson::dom::element& v_cur_elem);

	static void ClearStorage();

private:
	WorldFolderReader() = default;
	WorldFolderReader(const WorldFolderReader&) = delete;
	WorldFolderReader(WorldFolderReader&) = delete;
	~WorldFolderReader() = default;
};

SM_MANAGED_CODE
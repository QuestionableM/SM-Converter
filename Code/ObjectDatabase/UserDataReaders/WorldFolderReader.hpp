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
	WorldInstance(
		const SMUuid& wld_uuid,
		const std::wstring_view& wld_name,
		const std::wstring_view& wld_path,
		const std::wstring_view& wld_directory,
		const std::wstring_view& wld_image,
		std::uint64_t wld_workshop_id
	);

	WorldInstance(const WorldInstance&) = delete;
	WorldInstance(WorldInstance&) = delete;
	~WorldInstance() = default;

	SMUuid uuid;
	std::wstring name;
	std::wstring lower_name;

	std::wstring path;
	std::wstring directory;
	std::wstring preview_image;

	std::uint64_t workshop_id;
	std::uint8_t filter;
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
#pragma once

#include "UStd/UnmanagedFilesystem.hpp"
#include "Ustd/UnmanagedString.hpp"
#include "Ustd/UnmanagedVector.hpp"

#include "ObjectDatabase/Mods/Mod.hpp"
#include "FilterSettingsData.hpp"
#include "UserDataBase.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Uuid.hpp"
#include "Utils/Json.hpp"

SM_UNMANAGED_CODE

struct BlueprintInstance
{
	BlueprintInstance(
		SMUuid bp_uuid,
		const std::wstring_view& bp_name,
		const std::wstring_view& bp_path,
		const std::wstring_view& bp_directory,
		const std::wstring_view& bp_image,
		std::uint64_t bp_workshop_id
	);

	BlueprintInstance(
		const std::wstring_view& bp_name,
		const std::wstring_view& bp_path,
		const std::wstring_view& bp_directory,
		std::uint64_t bp_workshop_id
	);

	BlueprintInstance(const BlueprintInstance&) = delete;
	BlueprintInstance(BlueprintInstance&&) = delete;
	~BlueprintInstance() = default;

	SMUuid uuid;
	std::wstring name;
	//Used for more optimized search
	std::wstring lower_name;

	std::wstring path;
	std::wstring directory;
	std::wstring preview_image;

	std::uint64_t workshop_id;
	std::uint8_t filter;
};

class BlueprintFolderReader :
	public UserDataBase<BlueprintFolderReader, BlueprintInstance*>
{
public:
	static bool ShouldUseFilteredStorage();
	static void FilterStorage();

	static void LoadFromFile(const std::filesystem::path& path);
	static void LoadFromFolder(const std::wstring& folder, const simdjson::dom::element& v_cur_elem);
	
	static void ClearStorage();

private:
	BlueprintFolderReader() = default;
	BlueprintFolderReader(const BlueprintFolderReader&) = delete;
	BlueprintFolderReader(BlueprintFolderReader&&) = delete;
	~BlueprintFolderReader() = default;
};

SM_MANAGED_CODE
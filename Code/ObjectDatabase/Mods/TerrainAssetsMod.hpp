#pragma once

#include "Utils/clr_include.hpp"
#include "Mod.hpp"

SM_UNMANAGED_CODE

class TerrainAssetsMod : public SMMod
{
public:
	~TerrainAssetsMod() = default;
	TerrainAssetsMod(
		const std::wstring_view& name,
		const std::wstring_view& directory,
		const SMUuid& uuid,
		const std::uint64_t workshopId,
		const bool isLocal
	);

private:
	static bool GetValidAssetDatabaseFolder(const std::wstring& mod_path, std::wstring& r_asset_db);
	static bool GetAssetSetDatabaseFile(const std::wstring& asset_db_dir, std::wstring& r_asset_set);

public:
	inline ModType Type() const noexcept override { return ModType::TerrainAssets; }
	void LoadObjectDatabase() override;
};

SM_MANAGED_CODE
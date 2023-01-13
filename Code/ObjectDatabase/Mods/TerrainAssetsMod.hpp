#pragma once

#include "Mod.hpp"

class TerrainAssetsMod : public Mod
{
	static bool GetValidAssetDatabaseFolder(const std::wstring& mod_path, std::wstring& r_asset_db);
	static bool GetAssetSetDatabaseFile(const std::wstring& asset_db_dir, std::wstring& r_asset_set);
	static void LoadAssetSetDatabase(const std::wstring& path, Mod* pMod);

public:
	TerrainAssetsMod() = default;
	~TerrainAssetsMod() = default;

	inline ModType Type() const override { return ModType::TerrainAssets; }
	void LoadObjectDatabase() override;
};
#pragma once

#include "Mod.hpp"

#pragma unmanaged

class TerrainAssetsMod : public SMMod
{
	static bool GetValidAssetDatabaseFolder(const std::wstring& mod_path, std::wstring& r_asset_db);
	static bool GetAssetSetDatabaseFile(const std::wstring& asset_db_dir, std::wstring& r_asset_set);

public:
	~TerrainAssetsMod() = default;
	inline TerrainAssetsMod()
	{
		SMMod::ModStorage.insert(std::make_pair(m_Uuid, this));
		SMMod::ModVector.push_back(this);
	}

	inline ModType Type() const override { return ModType::TerrainAssets; }
	void LoadObjectDatabase() override;
};

#pragma managed
#pragma once

#include "Mod.hpp"

#pragma unmanaged

class TerrainAssetsMod : public SMMod
{
	static bool GetValidAssetDatabaseFolder(const std::wstring& mod_path, std::wstring& r_asset_db);
	static bool GetAssetSetDatabaseFile(const std::wstring& asset_db_dir, std::wstring& r_asset_set);

public:
	~TerrainAssetsMod() = default;

	using SMMod::SMMod;

	TerrainAssetsMod(const std::wstring& v_name, const std::wstring& v_directory, const SMUuid& v_uuid, unsigned long long v_workshop_id, bool v_isLocal)
		: SMMod(v_name, v_directory, v_uuid, v_workshop_id, v_isLocal)
	{
		SMMod::ModStorage.emplace(m_Uuid, this);
		SMMod::ModVector.push_back(this);
	}

	inline ModType Type() const noexcept override { return ModType::TerrainAssets; }
	void LoadObjectDatabase() override;
};

#pragma managed
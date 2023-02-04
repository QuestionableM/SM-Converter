#pragma once

#include "Mod.hpp"

#pragma unmanaged

class TerrainAssetsMod : public SMMod
{
	static bool GetValidAssetDatabaseFolder(const std::wstring& mod_path, std::wstring& r_asset_db);
	static bool GetAssetSetDatabaseFile(const std::wstring& asset_db_dir, std::wstring& r_asset_set);

public:
	~TerrainAssetsMod() = default;
	inline TerrainAssetsMod(const std::wstring& v_name, const std::wstring& v_directory, const SMUuid& v_uuid, const unsigned long long& v_workshop_id, const bool& v_isLocal)
	{
		this->m_Name = v_name;
		this->m_Directory = v_directory;
		this->m_Uuid = v_uuid;
		this->m_WorkshopId = v_workshop_id;
		this->m_isLocal = v_isLocal;

		SMMod::ModStorage.insert(std::make_pair(m_Uuid, this));
		SMMod::ModVector.push_back(this);
	}

	inline ModType Type() const override { return ModType::TerrainAssets; }
	void LoadObjectDatabase() override;
};

#pragma managed
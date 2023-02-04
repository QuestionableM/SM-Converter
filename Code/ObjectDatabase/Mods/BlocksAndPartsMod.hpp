#pragma once

#include "Mod.hpp"

#pragma unmanaged

class BlocksAndPartsMod : public SMMod
{
	static bool GetShapeSetDatabaseFile(const std::wstring& mod_folder, std::wstring& r_shapedb_path);

public:
	~BlocksAndPartsMod() = default;
	inline BlocksAndPartsMod(const std::wstring& v_name, const std::wstring& v_directory, const SMUuid& v_uuid, const unsigned long long& v_workshop_id, const bool& v_isLocal)
	{
		this->m_Name = v_name;
		this->m_Directory = v_directory;
		this->m_Uuid = v_uuid;
		this->m_WorkshopId = v_workshop_id;
		this->m_isLocal = v_isLocal;

		SMMod::ModStorage.insert(std::make_pair(m_Uuid, this));
		SMMod::ModVector.push_back(this);
	}

	inline ModType Type() const override { return ModType::BlocksAndParts; }
	void LoadObjectDatabase() override;
};

#pragma managed
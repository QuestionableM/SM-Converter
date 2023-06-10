#pragma once

#include "Mod.hpp"

#pragma unmanaged

class BlocksAndPartsMod : public SMMod
{
	static bool GetShapeSetDatabaseFile(const std::wstring& mod_folder, std::wstring& r_shapedb_path);

public:
	~BlocksAndPartsMod() = default;

	BlocksAndPartsMod(const std::wstring& v_name, const std::wstring& v_directory, const SMUuid& v_uuid, unsigned long long v_workshop_id, bool v_isLocal)
		: SMMod(v_name, v_directory, v_uuid, v_workshop_id, v_isLocal)
	{
		SMMod::ModStorage.emplace(m_Uuid, this);
		SMMod::ModVector.push_back(this);
	}

	inline ModType Type() const noexcept override { return ModType::BlocksAndParts; }
	void LoadObjectDatabase() override;
};

#pragma managed
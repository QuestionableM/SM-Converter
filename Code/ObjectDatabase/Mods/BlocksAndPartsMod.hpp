#pragma once

#include "Mod.hpp"

#pragma unmanaged

class BlocksAndPartsMod : public SMMod
{
	static bool GetShapeSetDatabaseFile(const std::wstring& mod_folder, std::wstring& r_shapedb_path);

public:
	~BlocksAndPartsMod() = default;
	inline BlocksAndPartsMod()
	{
		SMMod::ModStorage.insert(std::make_pair(m_Uuid, this));
		SMMod::ModVector.push_back(this);
	}

	inline ModType Type() const override { return ModType::BlocksAndParts; }
	void LoadObjectDatabase() override;
};

#pragma managed
#pragma once

#include "Mod.hpp"

#pragma unmanaged

class BlocksAndPartsMod : public SMMod
{
	static bool GetShapeSetDatabaseFile(const std::wstring& mod_folder, std::wstring& r_shapedb_path);
	static void LoadShapeSetDatabase(const std::wstring& path, SMMod* pMod);

public:
	BlocksAndPartsMod() = default;
	~BlocksAndPartsMod() = default;

	inline ModType Type() const override { return ModType::BlocksAndParts; }
	void LoadObjectDatabase() override;
};

#pragma managed
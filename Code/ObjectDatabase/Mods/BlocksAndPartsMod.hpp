#pragma once

#include "Mod.hpp"

class BlocksAndPartsMod : public Mod
{
	static bool GetShapeSetDatabaseFile(const std::wstring& mod_folder, std::wstring& r_shapedb_path);
	static void LoadShapeSetDatabase(const std::wstring& path, Mod* pMod);

public:
	BlocksAndPartsMod() = default;
	~BlocksAndPartsMod() = default;

	inline ModType Type() const override { return ModType::BlocksAndParts; }
	void LoadObjectDatabase() override;
};
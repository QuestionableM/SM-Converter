#pragma once

#include "Utils/clr_include.hpp"
#include "Mod.hpp"

SM_UNMANAGED_CODE

class GameDataMod : public SMMod
{
public:
	GameDataMod();
	~GameDataMod() = default;

	inline ModType Type() const noexcept override { return ModType::GameData; }
	void LoadObjectDatabase() override;
};

SM_MANAGED_CODE
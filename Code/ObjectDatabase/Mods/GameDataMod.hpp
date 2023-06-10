#pragma once

#include "Mod.hpp"

#pragma unmanaged

class GameDataMod : public SMMod
{
public:
	GameDataMod();
	~GameDataMod() = default;

	inline ModType Type() const noexcept override { return ModType::GameData; }
	void LoadObjectDatabase() override;
};

#pragma managed
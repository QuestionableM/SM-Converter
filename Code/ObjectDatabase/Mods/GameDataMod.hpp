#pragma once

#include "Mod.hpp"

#pragma unmanaged

class GameDataMod : public Mod
{
public:
	GameDataMod();
	~GameDataMod() = default;

	inline ModType Type() const override { return ModType::GameData; }
	void LoadObjectDatabase() override;
};

#pragma managed
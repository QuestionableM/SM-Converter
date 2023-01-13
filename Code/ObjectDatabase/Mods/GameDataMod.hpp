#pragma once

#include "Mod.hpp"

class GameDataMod : public Mod
{
public:
	GameDataMod();
	~GameDataMod() = default;

	inline ModType Type() const override { return ModType::GameData; }
	void LoadObjectDatabase() override;
};
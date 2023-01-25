#pragma once

#include "Mod.hpp"

#pragma unmanaged

class CustomGame : public SMMod
{
	bool CheckSurvivalContent();

public:
	inline ModType Type() const override { return ModType::CustomGame; }
	void LoadObjectDatabase() override;
};

#pragma managed
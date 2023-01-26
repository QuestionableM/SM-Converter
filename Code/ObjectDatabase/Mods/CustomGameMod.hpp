#pragma once

#include "Mod.hpp"

#pragma unmanaged

class CustomGame : public SMMod
{
	bool CheckSurvivalContent();

public:
	inline ModType Type() const override { return ModType::CustomGame; }
	void LoadObjectDatabase() override;

	inline bool ShouldUseGameContent() const { return m_shouldUseGameContent; }

private:
	bool m_shouldUseGameContent;
};

#pragma managed
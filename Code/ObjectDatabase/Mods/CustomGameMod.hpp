#pragma once

#include "Mod.hpp"

#pragma unmanaged

class CustomGame : public SMMod
{
	bool CheckSurvivalContent();

public:
	~CustomGame() = default;
	inline CustomGame()
	{
		SMMod::CustomGameStorage.insert(std::make_pair(m_Uuid, this));
		SMMod::CustomGameVector.push_back(this);
	}

	inline ModType Type() const override { return ModType::CustomGame; }
	void LoadObjectDatabase() override;

	bool m_shouldUseGameContent;
	bool m_shouldUseUserMods;
};

#pragma managed
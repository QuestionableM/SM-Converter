#pragma once

#include "Utils/clr_include.hpp"
#include "Mod.hpp"

SM_UNMANAGED_CODE

class CustomGame : public SMMod
{
	bool CheckSurvivalContent();

public:
	~CustomGame() = default;
	inline CustomGame(const std::wstring& v_name, const std::wstring& v_directory, const SMUuid& v_uuid, unsigned long long v_workshop_id, bool v_isLocal)
		: SMMod(v_name, v_directory, v_uuid, v_workshop_id, v_isLocal)
	{
		this->m_id = SMMod::CustomGameVector.size();

		SMMod::CustomGameStorage.emplace(m_Uuid, this);
		SMMod::CustomGameVector.push_back(this);
	}

	inline ModType Type() const noexcept override { return ModType::CustomGame; }
	void LoadObjectDatabase() override;

	bool m_shouldUseGameContent;
	bool m_shouldUseUserMods;

	std::size_t m_id;
};

SM_MANAGED_CODE
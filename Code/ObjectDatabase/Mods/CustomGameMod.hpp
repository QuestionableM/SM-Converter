#pragma once

#include "Mod.hpp"

#pragma unmanaged

class CustomGame : public SMMod
{
	bool CheckSurvivalContent();

public:
	~CustomGame() = default;
	inline CustomGame(const std::wstring& v_name, const std::wstring& v_directory, const SMUuid& v_uuid, const unsigned long long& v_workshop_id, const bool& v_isLocal)
	{
		this->m_Name = v_name;
		this->m_Directory = v_directory;
		this->m_Uuid = v_uuid;
		this->m_WorkshopId = v_workshop_id;
		this->m_isLocal = v_isLocal;

		this->m_id = SMMod::CustomGameVector.size();

		SMMod::CustomGameStorage.insert(std::make_pair(m_Uuid, this));
		SMMod::CustomGameVector.push_back(this);
	}

	inline ModType Type() const override { return ModType::CustomGame; }
	void LoadObjectDatabase() override;

	bool m_shouldUseGameContent;
	bool m_shouldUseUserMods;

	std::size_t m_id;
};

#pragma managed
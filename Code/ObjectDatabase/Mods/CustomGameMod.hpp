#pragma once

#include "Utils/clr_include.hpp"
#include "Mod.hpp"

SM_UNMANAGED_CODE

class CustomGame : public SMMod
{
public:
	~CustomGame() = default;
	CustomGame(
		const std::wstring_view& name,
		const std::wstring_view& directory,
		const SMUuid& uuid,
		std::uint64_t workshop_id,
		bool isLocal
	);

private:
	bool CheckSurvivalContent();

public:
	inline ModType Type() const noexcept override { return ModType::CustomGame; }
	void LoadObjectDatabase() override;

	bool m_shouldUseGameContent;
	bool m_shouldUseUserMods;

	std::size_t m_id;
};

SM_MANAGED_CODE
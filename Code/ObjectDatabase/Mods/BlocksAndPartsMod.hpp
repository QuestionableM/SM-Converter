#pragma once

#include "Utils/clr_include.hpp"
#include "Mod.hpp"

SM_UNMANAGED_CODE

class BlocksAndPartsMod : public SMMod
{
public:
	~BlocksAndPartsMod() = default;
	BlocksAndPartsMod(
		const std::wstring_view& name,
		const std::wstring_view& directory,
		const SMUuid& uuid,
		std::uint64_t workshop_id,
		bool isLocal
	);

private:
	static bool GetShapeSetDatabaseFile(const std::wstring& mod_folder, std::wstring& r_shapedb_path);

public:
	inline ModType Type() const noexcept override { return ModType::BlocksAndParts; }
	void LoadObjectDatabase() override;
};

SM_MANAGED_CODE
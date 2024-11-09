#pragma once

#include "Utils/clr_include.hpp"
#include "Entity.hpp"

SM_UNMANAGED_CODE

class SMTileClutter final : public SMEntityWithModel
{
public:
	SMTileClutter(ClutterData* pParent, Model* pModel);
	SMTileClutter(SMTileClutter&) = delete;
	SMTileClutter(const SMTileClutter&) = delete;
	~SMTileClutter() = default;

	float ScaleVariance() const noexcept;
	void SetColor(SMColor color);

	const SMUuid& GetUuid() const override;
	EntityType Type() const noexcept override;
	char* GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const override;
	std::string GetMtlName(std::size_t mIdx) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;

private:
	class ClutterData* m_parent;
	SMColor m_color = 0xffffff;
};

SM_MANAGED_CODE
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
	void SetColor(const SMColor color) noexcept;

	const SMUuid& GetUuid() const override;
	EntityType Type() const override;
	char* GetMtlNameCStr(const std::string_view& material, const std::size_t idx, char* pCString) const override;
	std::string GetMtlName(const std::size_t idx) const override;
	void FillTextureMap(EntityTextureMap& textureMap) const override;

private:
	class ClutterData* m_parent;
	SMColor m_color = 0xffffff;
};

SM_MANAGED_CODE
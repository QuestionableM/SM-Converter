#pragma once

#include "ObjectDatabase/ModelStorage.hpp"
#include "Entity.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Color.hpp"

SM_UNMANAGED_CODE

class SMHarvestable final : public SMEntityWithModelAndUuid
{
public:
	SMHarvestable(
		const HarvestableData* pParent,
		const SMEntityTransform& transform,
		Model* pModel,
		const SMColor color
	);

	SMHarvestable(const SMHarvestable&) = delete;
	SMHarvestable(SMHarvestable&) = delete;
	~SMHarvestable() = default;

	SMColor GetColor() const override;
	EntityType Type() const override;
	char* GetMtlNameCStr(const std::string_view& material, const std::size_t idx, char* pCString) const override;
	void FillTextureMap(EntityTextureMap& textureMap) const override;
	bool GetCanWrite(const std::string_view& name, const std::size_t idx) const override;

private:
	const HarvestableData* m_parent;
	SMColor m_color;
};

SM_MANAGED_CODE
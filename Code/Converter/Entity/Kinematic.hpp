#pragma once

#include "ObjectDatabase/ModelStorage.hpp"
#include "Entity.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Color.hpp"

SM_UNMANAGED_CODE

class SMKinematic final : public SMEntityWithModelAndUuid
{
public:
	SMKinematic(
		const KinematicData* pParent,
		const SMEntityTransform& transform,
		Model* pModel,
		const SMColor color
	);

	SMKinematic(const SMKinematic&) = delete;
	SMKinematic(SMKinematic&) = delete;
	~SMKinematic() = default;

	SMColor GetColor() const override;
	EntityType Type() const override;
	char* GetMtlNameCStr(const std::string_view& material, const std::size_t idx, char* pCString) const override;
	void FillTextureMap(EntityTextureMap& textureMap) const override;
	bool GetCanWrite(const std::string_view& name, const std::size_t idx) const override;

private:
	const KinematicData* m_parent;
	SMColor m_color;
};

SM_MANAGED_CODE
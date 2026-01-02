#pragma once

#include "ObjectDatabase/ModelStorage.hpp"
#include "Entity.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Color.hpp"

SM_UNMANAGED_CODE

class SMPart final : public SMEntityWithModelAndUuid
{
public:
	SMPart(
		const PartData* pParent,
		const glm::vec3& pos,
		Model* pModel,
		const SMColor color,
		const std::uint8_t rotation,
		const std::size_t index
	);

	SMPart(const SMPart&) = delete;
	SMPart(SMPart&) = delete;
	~SMPart() = default;

	std::size_t GetIndex() const override;
	SMColor GetColor() const override;
	EntityType Type() const override;
	char* GetMtlNameCStr(const std::string_view& material, const std::size_t idx, char* pCString) const override;
	void FillTextureMap(EntityTextureMap& textureMap) const override;
	glm::mat4 GetTransformMatrix() const override;
	bool GetCanWrite(const std::string_view& name, const std::size_t idx) const override;

private:
	const PartData* m_parent;
	std::size_t m_index;
	SMColor m_color;
	unsigned char m_xzRotation;
};

SM_MANAGED_CODE
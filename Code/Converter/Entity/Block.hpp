#pragma once

#include "Utils/clr_include.hpp"
#include "Entity.hpp"

SM_UNMANAGED_CODE

class SMBlock final : public SMEntityWithUuid
{
	friend class BlueprintConv;

public:
	SMBlock(
		SMBlock* other,
		const glm::vec3& pos,
		const glm::quat& rot,
		const glm::vec3& scale
	);

	SMBlock(
		const BlockData* pParent,
		const glm::vec3& pos,
		const glm::vec3& scale,
		const SMColor color,
		const std::uint8_t rotation,
		const std::size_t index
	);

	SMBlock(const SMBlock&) = delete;
	SMBlock(SMBlock&) = delete;

	~SMBlock() = default;

	std::size_t GetIndex() const override;
	SMColor GetColor() const override;
	EntityType Type() const override;
	char* GetMtlNameCStr(const std::string_view& material, const std::size_t idx, char* pCString) const override;
	std::string GetMtlName(const std::size_t idx) const override;
	void FillTextureMap(EntityTextureMap& textureMap) const override;
	void WriteObjectToFile(std::ofstream& file, WriterOffsetData& offset, const glm::mat4& transform) const override;
	glm::mat4 GetTransformMatrix() const override;

private:
	const BlockData* m_parent;
	std::size_t m_index;
	SMColor m_color;
	std::uint8_t m_xzRotation;
};

SM_MANAGED_CODE
#pragma once

#include "Entity.hpp"

SM_UNMANAGED_CODE

class SMWedge final : public SMEntityWithUuid
{
	friend class BlueprintConv;

public:
	SMWedge(
		const WedgeData* pParent,
		const BlockData* pBlockParent,
		const glm::vec3& pos,
		const glm::vec3& scale,
		const SMColor color,
		const std::uint8_t rotation,
		const std::size_t index
	);

	SMWedge(const SMWedge&) = delete;
	SMWedge(SMWedge&) = delete;
	~SMWedge() = default;

	std::size_t GetIndex() const override;
	SMColor GetColor() const override;
	EntityType Type() const override;
	char* GetMtlNameCStr(const std::string_view& material, const std::size_t idx, char* pCString) const override;
	std::string GetMtlName(const std::size_t idx) const override;
	void FillTextureMap(EntityTextureMap& textureMap) const override;
	void WriteObjectToFile(std::ofstream& file, WriterOffsetData& offset, const glm::mat4& transform) const override;
	glm::mat4 GetTransformMatrix() const override;

private:
	const WedgeData* m_parent;
	const BlockData* m_parentBlock;

	std::size_t m_index;
	SMColor m_color;
	std::uint8_t m_xzRotation;
};

SM_MANAGED_CODE
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

	inline std::size_t GetIndex() const override { return m_index; }
	inline SMColor GetColor() const override { return m_color; }
	inline EntityType Type() const noexcept override { return EntityType::Wedge; }
	char* GetMtlNameCStr(const std::string& matName, std::size_t idx, char* ptr) const override;
	std::string GetMtlName(std::size_t idx) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& outTexMap) const override;
	void WriteObjectToFile(std::ofstream& file, WriterOffsetData& offset, const glm::mat4& transformMatrix) const override;
	glm::mat4 GetTransformMatrix() const override;

private:
	const WedgeData* m_parent;
	const BlockData* m_parentBlock;

	std::size_t m_index;
	SMColor m_color;
	std::uint8_t m_xzRotation;
};

SM_MANAGED_CODE
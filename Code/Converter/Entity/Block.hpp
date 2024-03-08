#pragma once

#include "Utils/clr_include.hpp"
#include "Entity.hpp"

SM_UNMANAGED_CODE

class SMBlock : public SMEntityWithUuid
{
	friend class BlueprintConv;

public:
	inline SMBlock(SMBlock* other, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale)
		: SMEntityWithUuid(other->m_parent->m_uuid, pos, scale),
		m_parent(other->m_parent),
		m_index(0),
		m_color(other->m_color),
		m_xzRotation(other->m_xzRotation) {}

	inline SMBlock(const BlockData* pParent,
		const glm::vec3& pos,
		const glm::vec3& scale,
		SMColor color,
		unsigned char rotation,
		std::size_t index)
		: SMEntityWithUuid(pParent->m_uuid, pos, scale),
		m_parent(pParent),
		m_index(index),
		m_color(color),
		m_xzRotation(rotation) {}

	SMBlock(const SMBlock&) = delete;
	SMBlock(SMBlock&) = delete;
	~SMBlock() = default;

	inline std::size_t GetIndex() const override { return m_index; }
	inline SMColor GetColor() const override { return m_color; }
	inline EntityType Type() const noexcept override { return EntityType::Block; }
	char* GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const override;
	std::string GetMtlName(std::size_t v_idx) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	void WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const override;
	glm::mat4 GetTransformMatrix() const override;

private:
	const BlockData* m_parent;
	std::size_t m_index;
	SMColor m_color;
	unsigned char m_xzRotation;
};

SM_MANAGED_CODE
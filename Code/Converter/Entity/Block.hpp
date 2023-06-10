#pragma once

#include "Entity.hpp"

#pragma unmanaged

class SMBlock : public SMEntity
{
	friend class BlueprintConv;

public:
	inline SMBlock(const BlockData* pParent,
		const glm::vec3& bounds,
		SMColor color,
		unsigned char v_rotation,
		std::size_t index)
	{
		this->m_parent = pParent;
		this->m_uuid = pParent->m_uuid;
		this->m_bounds = bounds;
		this->m_color = color;

		this->m_xzRotation = v_rotation;
		this->m_index = index;
	}

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
	SMColor m_color;
	const BlockData* m_parent;
	glm::vec3 m_bounds;
	unsigned char m_xzRotation;
	std::size_t m_index;
};

#pragma managed
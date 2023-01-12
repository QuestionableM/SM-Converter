#pragma once

#include "Entity.hpp"

#pragma unmanaged

class Block : public SMEntity
{
public:
	inline Block(BlockData* pParent, const glm::vec3& bounds, const SMColor& color, const int& xAxis, const int& zAxis)
	{
		this->m_parent = pParent;
		this->m_uuid = pParent->Uuid;
		this->m_bounds = bounds;
		this->m_color = color;
		this->m_xAxis = xAxis;
		this->m_zAxis = zAxis;
	}

	Block(const Block&) = delete;
	Block(Block&) = delete;
	~Block() = default;

	inline EntityType Type() const override { return EntityType::Block; }
	std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	void WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const override;
	glm::mat4 GetTransformMatrix() const override;

private:
	SMColor m_color;
	BlockData* m_parent;
	glm::vec3 m_bounds;
	int m_xAxis;
	int m_zAxis;
};

#pragma managed
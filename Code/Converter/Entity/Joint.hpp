#pragma once

#include "ObjectDatabase\ModelStorage.hpp"
#include "Utils\Color.hpp"
#include "Entity.hpp"

class Joint : public SMEntity
{
public:
	inline Joint(PartData* pParent, Model* pModel, const SMColor& color, const int& xAxis, const int& zAxis, const std::size_t& index)
	{
		this->m_parent = pParent;
		this->m_uuid = pParent->Uuid;
		this->m_model = pModel;
		this->m_color = color;
		this->m_xAxis = xAxis;
		this->m_zAxis = zAxis;
		this->m_index = index;
	}

	Joint(const Joint&) = delete;
	Joint(Joint&) = delete;
	~Joint() = default;

	inline std::size_t GetIndex() const override { return m_index; }
	inline SMColor GetColor() const override { return m_color; }
	inline EntityType Type() const override { return EntityType::Joint; }
	std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	glm::mat4 GetTransformMatrix() const override;

private:
	SMColor m_color;
	PartData* m_parent;
	int m_xAxis;
	int m_zAxis;
	std::size_t m_index;
};
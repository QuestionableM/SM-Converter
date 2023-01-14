#pragma once

#include "ObjectDatabase\ModelStorage.hpp"
#include "Utils\Color.hpp"
#include "Entity.hpp"

class Part : public SMEntity
{
	SMColor m_color;
	PartData* m_parent;
	int m_xAxis;
	int m_zAxis;

public:
	inline Part(PartData* pParent, Model* pModel, const SMColor& color, const int& xAxis, const int& zAxis)
	{
		this->m_parent = pParent;
		this->m_uuid = pParent->Uuid;
		this->m_model = pModel;
		this->m_color = color;
		this->m_xAxis = xAxis;
		this->m_zAxis = zAxis;
	}

	Part(const Part&) = delete;
	Part(Part&) = delete;
	~Part() = default;

	inline SMColor GetColor() const override { return this->m_color; }
	inline EntityType Type() const override { return EntityType::Part; }
	std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	glm::mat4 GetTransformMatrix() const override;
};
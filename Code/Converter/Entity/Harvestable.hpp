#pragma once

#include "ObjectDatabase\ModelStorage.hpp"
#include "Utils\Color.hpp"
#include "Entity.hpp"

class Harvestable : public SMEntity
{
public:
	inline Harvestable(HarvestableData* pParent, Model* pModel, const SMColor& color)
	{
		this->m_parent = pParent;
		this->m_uuid = pParent->Uuid;
		this->m_model = pModel;
		this->m_color = color;
	}

	Harvestable(const Harvestable&) = delete;
	Harvestable(Harvestable&) = delete;
	~Harvestable() = default;

	inline const SMColor& GetColor() const { return m_color; }

	inline EntityType Type() const { return EntityType::Harvestable; }
	std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;

private:
	SMColor m_color;
	class HarvestableData* m_parent;
};
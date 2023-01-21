#pragma once

#include "ObjectDatabase\ModelStorage.hpp"
#include "Utils\Color.hpp"
#include "Entity.hpp"

#pragma unmanaged

class SMHarvestable : public SMEntity
{
public:
	inline SMHarvestable(HarvestableData* pParent, Model* pModel, const SMColor& color)
	{
		this->m_parent = pParent;
		this->m_uuid = pParent->m_uuid;
		this->m_model = pModel;
		this->m_color = color;
	}

	SMHarvestable(const SMHarvestable&) = delete;
	SMHarvestable(SMHarvestable&) = delete;
	~SMHarvestable() = default;

	inline SMColor GetColor() const override { return m_color; }
	inline EntityType Type() const { return EntityType::Harvestable; }
	char* GetMtlNameCStr(const std::string& v_mat_name, const std::size_t& v_idx, char* v_ptr) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	bool GetCanWrite(const std::string& name, const std::size_t& v_idx) const override;

private:
	SMColor m_color;
	class HarvestableData* m_parent;
};

#pragma managed
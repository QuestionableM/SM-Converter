#pragma once

#include "Entity.hpp"

class TileClutter : public SMEntity
{
public:
	inline TileClutter(ClutterData* pParent, Model* pModel)
	{
		this->m_parent = pParent;
		this->m_uuid = pParent->Uuid;
		this->m_model = pModel;
	}

	TileClutter(TileClutter&) = delete;
	TileClutter(const TileClutter&) = delete;
	~TileClutter() = default;

	inline const float& ScaleVariance() const { return m_parent->ScaleVariance; }

	inline void SetColor(const SMColor& color) { this->m_color = color; }

	inline EntityType Type() const override { return EntityType::Clutter; }
	std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;

private:
	class ClutterData* m_parent;
	SMColor m_color = 0xffffff;
};
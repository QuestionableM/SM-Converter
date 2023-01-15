#pragma once

#include "Entity.hpp"

#include "Utils\Color.hpp"
#include "Utils\Uuid.hpp"

#include <unordered_map>

class SMAsset : public SMEntity
{
	class AssetData* m_parent;

	using ColorMap = std::unordered_map<std::wstring, SMColor>;
	ColorMap m_colors;

public:
	inline SMAsset(AssetData* pParent, Model* pModel, const ColorMap& color_map)
	{
		this->m_parent = pParent;
		this->m_uuid = pParent->Uuid;
		this->m_model = pModel;
		this->m_colors = color_map;
	}

	SMAsset(const SMAsset&) = delete;
	SMAsset(SMAsset&&) = delete;
	~SMAsset() = default;

	SMColor GetColor(const std::wstring& color) const;
	inline const ColorMap& GetMaterials() const { return m_colors; }

	inline EntityType Type() const override { return EntityType::Asset; }
	std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
};
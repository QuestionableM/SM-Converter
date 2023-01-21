#pragma once

#include "Entity.hpp"

#include "UStd\UnmanagedUnorderedMap.hpp"

#include "Utils\Color.hpp"
#include "Utils\Uuid.hpp"

#pragma unmanaged

class SMAsset : public SMEntity
{
	class AssetData* m_parent;

	using ColorMap = std::unordered_map<std::string, SMColor>;
	ColorMap m_colors;

public:
	inline SMAsset(AssetData* pParent, Model* pModel, const ColorMap& color_map)
	{
		this->m_parent = pParent;
		this->m_uuid = pParent->m_uuid;
		this->m_model = pModel;
		this->m_colors = color_map;
	}

	SMAsset(const SMAsset&) = delete;
	SMAsset(SMAsset&&) = delete;
	~SMAsset() = default;

	SMColor GetColor(const std::string& color) const;
	inline const ColorMap& GetMaterials() const { return m_colors; }

	inline EntityType Type() const override { return EntityType::Asset; }
	char* GetMtlNameCStr(const std::string& v_mat_name, const std::size_t& v_idx, char* v_ptr) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	bool GetCanWrite(const std::string& name, const std::size_t& v_idx) const override;
};

#pragma managed
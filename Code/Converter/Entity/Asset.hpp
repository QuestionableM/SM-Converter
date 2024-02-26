#pragma once

#include "Entity.hpp"

#include "UStd/UnmanagedUnorderedMap.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Color.hpp"
#include "Utils/Uuid.hpp"

SM_UNMANAGED_CODE

class SMAsset : public SMEntityWithModelAndUuid
{
	using ColorMap = std::unordered_map<std::string, SMColor>;

public:
	inline SMAsset(
		const AssetData* pParent,
		const SMEntityTransform& transform,
		Model* pModel,
		ColorMap&& color_map)
		: SMEntityWithModelAndUuid(pParent->m_uuid, pModel, transform),
		m_parent(pParent),
		m_colors(std::move(color_map)) {}

	SMAsset(const SMAsset&) = delete;
	SMAsset(SMAsset&&) = delete;
	~SMAsset() = default;

	SMColor GetColor(const std::string& color) const;
	inline const ColorMap& GetMaterials() const { return m_colors; }

	inline EntityType Type() const noexcept override { return EntityType::Asset; }
	char* GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	bool GetCanWrite(const std::string& name, std::size_t v_idx) const override;

private:
	const AssetData* m_parent;
	ColorMap m_colors;
};

SM_MANAGED_CODE
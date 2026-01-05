#pragma once

#include "Entity.hpp"

#include "UStd/UnmanagedUnorderedMap.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Hashing.hpp"
#include "Utils/Color.hpp"
#include "Utils/Uuid.hpp"

SM_UNMANAGED_CODE

class SMAsset final : public SMEntityWithModelAndUuid
{
	using ColorMap = std::unordered_map<std::string, SMColor, Hashing::StringHasher, std::equal_to<>>;

public:
	SMAsset(
		const AssetData* pParent,
		const SMEntityTransform& transform,
		Model* pModel,
		ColorMap&& colorMap
	);

	SMAsset(const SMAsset&) = delete;
	SMAsset(SMAsset&&) = delete;

	~SMAsset() = default;

	SMColor GetColor(const std::string_view& color) const;
	inline const ColorMap& GetMaterials() const { return m_colors; }

	inline EntityType Type() const noexcept override { return EntityType::Asset; }
	char* GetMtlNameCStr(const std::string_view& material, const std::size_t idx, char* pCString) const override;
	void FillTextureMap(EntityTextureMap& textureMap) const override;
	bool GetCanWrite(const std::string_view& name, const std::size_t idx) const override;

private:
	const AssetData* m_parent;
	ColorMap m_colors;
};

SM_MANAGED_CODE
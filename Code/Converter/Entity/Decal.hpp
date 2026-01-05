#pragma once

#include "ObjectDatabase/ObjectData.hpp"
#include "Entity.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Color.hpp"

SM_UNMANAGED_CODE

class SMDecal final : public SMEntity
{
public:
	SMDecal(const DecalData* pData, const SMEntityTransform& transform, const SMColor color);
	SMDecal(const SMDecal&) = delete;
	SMDecal(SMDecal&) = delete;
	~SMDecal() = default;
	
	EntityType Type() const override;

	char* GetMtlNameCStr(const std::string_view& material, const std::size_t idx, char* pCString) const override;
	std::string GetMtlName(const std::size_t idx) const override;
	void FillTextureMap(EntityTextureMap& textureMap) const override;
	void WriteObjectToFile(std::ofstream& file, WriterOffsetData& offset, const glm::mat4& transform) const override;

private:
	const DecalData* m_data;
	SMColor m_color;
};

SM_MANAGED_CODE
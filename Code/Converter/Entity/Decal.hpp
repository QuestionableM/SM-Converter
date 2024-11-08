#pragma once

#include "ObjectDatabase/ObjectData.hpp"
#include "Entity.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Color.hpp"

SM_UNMANAGED_CODE

class SMDecal final : public SMEntity
{
public:
	~SMDecal() = default;
	
	SMDecal(const SMDecal&) = delete;
	SMDecal(SMDecal&) = delete;
	
	SMDecal(const DecalData* data_ptr, const SMEntityTransform& transform, SMColor color);
	EntityType Type() const noexcept override;

	char* GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const override;
	std::string GetMtlName(std::size_t mIdx) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	void WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const override;

private:
	const DecalData* m_data;
	SMColor m_color;
};

SM_MANAGED_CODE
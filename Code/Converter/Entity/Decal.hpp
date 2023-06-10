#pragma once

#include "ObjectDatabase\ObjectData.hpp"
#include "Utils\Color.hpp"
#include "Entity.hpp"

#pragma unmanaged

class SMDecal : public SMEntity
{
public:
	~SMDecal() = default;
	SMDecal(const SMDecal&) = delete;
	SMDecal(SMDecal&) = delete;
	inline SMDecal(const DecalData* data_ptr, SMColor color)
	{
		this->m_data = data_ptr;
		this->m_color = color;
	}

	inline EntityType Type() const noexcept override { return EntityType::Decal; }

	char* GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const override;
	std::string GetMtlName(std::size_t mIdx) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	void WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const override;

private:
	const DecalData* m_data;
	SMColor m_color;
};

#pragma managed
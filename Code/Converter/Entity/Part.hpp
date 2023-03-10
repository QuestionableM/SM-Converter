#pragma once

#include "ObjectDatabase\ModelStorage.hpp"
#include "Utils\Color.hpp"
#include "Entity.hpp"

#pragma unmanaged

class SMPart : public SMEntity
{
public:
	inline SMPart(const PartData* pParent,
		Model* pModel,
		const SMColor& color,
		const unsigned char& v_rotation,
		const std::size_t& index)
	{
		this->m_parent = pParent;
		this->m_uuid = pParent->m_uuid;
		this->m_model = pModel;
		this->m_color = color;

		this->m_xzRotation = v_rotation;
		this->m_index = index;
	}

	SMPart(const SMPart&) = delete;
	SMPart(SMPart&) = delete;
	~SMPart() = default;

	inline std::size_t GetIndex() const override { return m_index; }
	inline SMColor GetColor() const override { return this->m_color; }
	inline EntityType Type() const override { return EntityType::Part; }
	char* GetMtlNameCStr(const std::string& v_mat_name, const std::size_t& v_idx, char* v_ptr) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	glm::mat4 GetTransformMatrix() const override;
	bool GetCanWrite(const std::string& name, const std::size_t& v_idx) const override;

private:
	SMColor m_color;
	const PartData* m_parent;
	unsigned char m_xzRotation;
	std::size_t m_index;
};

#pragma managed
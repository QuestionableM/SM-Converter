#pragma once

#include "ObjectDatabase\ModelStorage.hpp"
#include "Utils\Color.hpp"
#include "Entity.hpp"

#pragma unmanaged

class SMPart : public SMEntityWithModelAndUuid
{
public:
	inline SMPart(const PartData* pParent,
		const glm::vec3& pos,
		Model* pModel,
		SMColor color,
		unsigned char rotation,
		std::size_t index)
		: SMEntityWithModelAndUuid(pParent->m_uuid, pModel, pos),
		m_parent(pParent),
		m_index(index),
		m_color(color),
		m_xzRotation(rotation) {}

	SMPart(const SMPart&) = delete;
	SMPart(SMPart&) = delete;
	~SMPart() = default;

	inline std::size_t GetIndex() const override { return m_index; }
	inline SMColor GetColor() const override { return this->m_color; }
	inline EntityType Type() const noexcept override { return EntityType::Part; }
	char* GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	glm::mat4 GetTransformMatrix() const override;
	bool GetCanWrite(const std::string& name, std::size_t v_idx) const override;

private:
	const PartData* m_parent;
	std::size_t m_index;
	SMColor m_color;
	unsigned char m_xzRotation;
};

#pragma managed
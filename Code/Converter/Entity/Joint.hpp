#pragma once

#include "ObjectDatabase\ModelStorage.hpp"
#include "Utils\Color.hpp"
#include "Entity.hpp"

#pragma unmanaged

class SMJoint : public SMEntity
{
public:
	inline SMJoint(const PartData* pParent, Model* pModel, SMColor color, unsigned char v_rotation, std::size_t index)
	{
		this->m_parent = pParent;
		this->m_uuid = pParent->m_uuid;
		this->m_model = pModel;
		this->m_color = color;
		
		this->m_xzRotation = v_rotation;
		this->m_index = index;
	}

	SMJoint(const SMJoint&) = delete;
	SMJoint(SMJoint&) = delete;
	~SMJoint() = default;

	inline std::size_t GetIndex() const override { return m_index; }
	inline SMColor GetColor() const override { return m_color; }
	inline EntityType Type() const noexcept override { return EntityType::Joint; }
	char* GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	glm::mat4 GetTransformMatrix() const override;
	bool GetCanWrite(const std::string& name, std::size_t v_idx) const override;

private:
	SMColor m_color;
	const PartData* m_parent;
	unsigned char m_xzRotation;
	std::size_t m_index;
};

#pragma managed
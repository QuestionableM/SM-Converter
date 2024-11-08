#pragma once

#include "ObjectDatabase/ModelStorage.hpp"
#include "Entity.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Color.hpp"

SM_UNMANAGED_CODE

class SMPart : public SMEntityWithModelAndUuid
{
public:
	SMPart(
		const PartData* pParent,
		const glm::vec3& pos,
		Model* pModel,
		SMColor color,
		unsigned char rotation,
		std::size_t index
	);

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

SM_MANAGED_CODE
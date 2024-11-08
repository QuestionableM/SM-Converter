#pragma once

#include "ObjectDatabase/ModelStorage.hpp"
#include "Entity.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Color.hpp"

SM_UNMANAGED_CODE

class SMKinematic : public SMEntityWithModelAndUuid
{
public:
	SMKinematic(
		const KinematicData* pParent,
		const SMEntityTransform& transform,
		Model* pModel,
		SMColor color
	);

	SMKinematic(const SMKinematic&) = delete;
	SMKinematic(SMKinematic&) = delete;
	~SMKinematic() = default;

	inline SMColor GetColor() const override { return m_color; }
	inline EntityType Type() const noexcept override { return EntityType::Kinematic; }
	char* GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	bool GetCanWrite(const std::string& name, std::size_t v_idx) const override;

private:
	const KinematicData* m_parent;
	SMColor m_color;
};

SM_MANAGED_CODE
#pragma once

#include "ObjectDatabase\ModelStorage.hpp"
#include "Utils\Color.hpp"
#include "Entity.hpp"

#pragma unmanaged

class SMKinematic : public SMEntity
{
public:
	inline SMKinematic(KinematicData* v_km_data, Model* v_model, const SMColor& v_color)
	{
		this->m_parent = v_km_data;
		this->m_uuid = m_parent->m_uuid;
		this->m_model = v_model;
		this->m_color = v_color;
	}

	SMKinematic(const SMKinematic&) = delete;
	SMKinematic(SMKinematic&) = delete;
	~SMKinematic() = default;

	inline SMColor GetColor() const override { return m_color; }
	inline EntityType Type() const { return EntityType::Kinematic; }
	char* GetMtlNameCStr(const std::string& v_mat_name, const std::size_t& v_idx, char* v_ptr) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	bool GetCanWrite(const std::string& name, const std::size_t& v_idx) const override;

private:
	SMColor m_color;
	class KinematicData* m_parent;
};

#pragma managed
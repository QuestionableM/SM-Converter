#pragma once

#include "Entity.hpp"

#include <vector>
#include <string>

class SMBody : public SMEntity
{
public:
	inline SMBody(const std::string& name)
	{
		this->m_body_name = name;
	}

	inline ~SMBody()
	{
		for (std::size_t a = 0; a < m_objects.size(); a++)
			delete m_objects[a];
	}

	inline void Add(SMEntity* v_entity)
	{
		m_objects.push_back(v_entity);
	}

	inline EntityType Type() const override { return EntityType::Body; }
	std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const override { return ""; }
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;

private:
	std::string m_body_name;
	std::vector<SMEntity*> m_objects = {};
};
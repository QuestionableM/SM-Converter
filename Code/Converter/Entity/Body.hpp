#pragma once

#include "Entity.hpp"

#include <vector>
#include <string>

#pragma unmanaged

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
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	void WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const;
	void CalculateCenterPoint(glm::vec3& v_input) const override;
	inline std::size_t GetAmountOfObjects() const override { return m_objects.size(); }

private:
	std::string m_body_name;
	std::vector<SMEntity*> m_objects = {};
};

#pragma managed
#pragma once

#include "Entity.hpp"

#include "Converter\ConvertSettings.hpp"

#include "UStd\UnmanagedFstream.hpp"
#include "UStd\UnmanagedString.hpp"
#include "UStd\UnmanagedVector.hpp"

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

	inline void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override
	{
		for (const SMEntity* v_entity : m_objects)
			v_entity->FillTextureMap(tex_map);
	}

	inline void WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const
	{
		std::string v_group_name = "o ";
		v_group_name.append(m_body_name);

		if (BlueprintConverterSettings::SeparationType == 1)
		{
			for (std::size_t a = 0; a < m_objects.size(); a++)
			{
				std::string v_final_name = v_group_name;
				v_final_name.append(1, '_');
				v_final_name.append(std::to_string(a + 1));
				v_final_name.append(1, '\n');
				file.write(v_final_name.c_str(), v_final_name.size());

				m_objects[a]->WriteObjectToFile(file, mOffset, transform_matrix);
			}
		} 
		else
		{
			v_group_name.append(1, '\n');
			file.write(v_group_name.c_str(), v_group_name.size());

			for (const SMEntity* v_entity : m_objects)
				v_entity->WriteObjectToFile(file, mOffset, transform_matrix);
		}
	}

	inline void CalculateCenterPoint(glm::vec3& v_input) const override
	{
		for (const SMEntity* v_entity : m_objects)
			v_entity->CalculateCenterPoint(v_input);
	}

	inline std::size_t GetAmountOfObjects() const override { return m_objects.size(); }

private:
	std::string m_body_name;
	std::vector<SMEntity*> m_objects = {};
};

#pragma managed
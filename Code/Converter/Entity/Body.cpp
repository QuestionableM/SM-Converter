#include "Body.hpp"

#include "Utils\UnmanagedFstream.hpp"

#pragma unmanaged

void SMBody::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	for (const SMEntity* v_entity : m_objects)
		v_entity->FillTextureMap(tex_map);
}

void SMBody::WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const
{
	const std::string v_group_name = "o " + m_body_name + "\n";
	file.write(v_group_name.c_str(), v_group_name.size());

	for (const SMEntity* v_entity : m_objects)
		v_entity->WriteObjectToFile(file, mOffset, transform_matrix);
}

void SMBody::CalculateCenterPoint(glm::vec3& v_input) const
{
	for (const SMEntity* v_entity : m_objects)
		v_entity->CalculateCenterPoint(v_input);
}
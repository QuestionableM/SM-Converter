#include "Joint.hpp"

#include "ObjectDatabase\ObjectRotations.hpp"
#include "ObjectDatabase\MaterialManager.hpp"
#include "ObjectDatabase\ObjectData.hpp"

#include "Utils\Console.hpp"

#pragma unmanaged

char* SMJoint::GetMtlNameCStr(const std::string& v_mat_name, const std::size_t& v_idx, char* v_ptr) const
{
	v_ptr = m_uuid.ToCString(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = m_color.StringHexCStr(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = String::FromInteger<std::size_t>(v_idx + 1, v_ptr);
	*v_ptr++ = ' ';

	return MaterialManager::GetMaterialACStr(v_mat_name, v_ptr);
}

void SMJoint::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	const std::string mtl_first_part = m_uuid.ToString() + " " + m_color.StringHex() + " ";
	for (std::size_t a = 0; a < m_model->subMeshData.size(); a++)
	{
		const SubMeshData* pSubMesh = m_model->subMeshData[a];
	 	const SMTextureList* v_tex_list = m_parent->m_textures->GetTexList(pSubMesh->m_MaterialName, a);
		if (!v_tex_list) continue;

		const std::string v_mat_idx = MaterialManager::GetMaterialA(v_tex_list->material);
		const std::string v_mtl_name = mtl_first_part + std::to_string(a + 1) + " " + v_mat_idx;

		if (tex_map.find(v_mtl_name) != tex_map.end())
			continue;

		ObjectTexData v_obj_tex_data;
		v_obj_tex_data.m_textures = *v_tex_list;
		v_obj_tex_data.m_tex_color = m_color;

		tex_map.insert(std::make_pair(v_mtl_name, v_obj_tex_data));
	}
}

glm::mat4 SMJoint::GetTransformMatrix() const
{
	const glm::mat4 joint_rotation = Rotations::GetRotationMatrix(m_xAxis, m_zAxis);
	const glm::vec3 pos_offset = Rotations::GetOffsetPosition(m_xAxis, m_zAxis);

	glm::mat4 model_matrix(1.0f);
	model_matrix *= glm::translate(m_position + pos_offset);
	model_matrix *= joint_rotation;
	model_matrix *= glm::translate(m_parent->m_bounds / 2.0f);

	return model_matrix;
}

bool SMJoint::GetCanWrite(const std::string& name, const std::size_t& v_idx) const
{
	const SMTextureList* v_tex_list = m_parent->m_textures->GetTexList(name, v_idx);
	if (!v_tex_list) return false;

	return !v_tex_list->is_shadow_only;
}
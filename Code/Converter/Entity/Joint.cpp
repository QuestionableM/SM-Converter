#include "Joint.hpp"

#include "ObjectDatabase\ObjectRotations.hpp"
#include "ObjectDatabase\MaterialManager.hpp"
#include "ObjectDatabase\ObjectData.hpp"

#include "Utils\Console.hpp"

#pragma unmanaged

char* SMJoint::GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const
{
	v_ptr = m_uuid.ToCString(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = m_color.StringHexCStr(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = String::FromInteger<std::size_t>(v_idx + 1, v_ptr);
	*v_ptr++ = ' ';

	const SMTextureList* v_tex_data = m_parent->m_textures->GetTexList(v_mat_name, v_idx);
	if (v_tex_data)
		return MaterialManager::GetMaterialACStr(v_tex_data->material, v_ptr);

	*v_ptr++ = 'm';
	*v_ptr++ = '1';

	return v_ptr;
}

void SMJoint::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	const std::string mtl_first_part = m_uuid.ToString() + " " + m_color.StringHex() + " ";

	const std::size_t v_modelSubMeshCount = m_model->m_subMeshData.size();
	for (std::size_t a = 0; a < v_modelSubMeshCount; a++)
	{
		const SubMeshData& v_curSubMesh = m_model->m_subMeshData[a];
	 	const SMTextureList* v_pTexList = m_parent->m_textures->GetTexList(v_curSubMesh.m_materialName, a);
		if (!v_pTexList) continue;

		const std::string v_mat_idx = MaterialManager::GetMaterialA(v_pTexList->material);
		std::string v_mtlName = mtl_first_part + std::to_string(a + 1) + " " + v_mat_idx;

		if (tex_map.find(v_mtlName) != tex_map.end())
			continue;

		tex_map.emplace(std::move(v_mtlName), ObjectTexDataConstructInfo(*v_pTexList, m_color));
	}
}

glm::mat4 SMJoint::GetTransformMatrix() const
{
	const glm::mat4 joint_rotation = Rotations::GetRotationMatrix(m_xzRotation);
	const glm::vec3 pos_offset = Rotations::GetOffsetPosition(m_xzRotation);

	glm::mat4 model_matrix(1.0f);
	model_matrix *= glm::translate(m_position + pos_offset);
	model_matrix *= joint_rotation;
	model_matrix *= glm::translate(m_parent->m_bounds / 2.0f);

	return model_matrix;
}

bool SMJoint::GetCanWrite(const std::string& name, std::size_t v_idx) const
{
	const SMTextureList* v_tex_list = m_parent->m_textures->GetTexList(name, v_idx);
	if (!v_tex_list) return false;

	return !v_tex_list->is_shadow_only;
}
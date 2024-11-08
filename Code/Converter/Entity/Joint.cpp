#include "Joint.hpp"

#include "ObjectDatabase\ObjectRotations.hpp"
#include "ObjectDatabase\MaterialManager.hpp"
#include "ObjectDatabase\ObjectData.hpp"

#include "Utils\Console.hpp"

#pragma unmanaged

SMJoint::SMJoint(
	const PartData* pParent,
	const glm::vec3& pos,
	Model* pModel,
	SMColor color,
	unsigned char rotation,
	std::size_t index
) :
	SMEntityWithModelAndUuid(pParent->m_uuid, pModel, pos),
	m_parent(pParent),
	m_index(index),
	m_color(color),
	m_xzRotation(rotation)
{}

char* SMJoint::GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const
{
	v_ptr = m_uuid.toCString(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = m_color.StringHexCStr(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = String::FromInteger<std::size_t>(v_idx + 1, v_ptr);
	*v_ptr++ = ' ';

	const SMTextureList* v_tex_data = m_parent->m_textures->getTexList(v_mat_name, v_idx);
	if (v_tex_data)
		return MaterialManager::GetMaterialACStr(v_tex_data->m_material, v_ptr);

	*v_ptr++ = 'm';
	*v_ptr++ = '1';

	return v_ptr;
}

void SMJoint::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	std::string v_mtlFirstPart(m_uuid.toString());
	v_mtlFirstPart.append(1, ' ');
	m_color.appendStringHex(v_mtlFirstPart);
	v_mtlFirstPart.append(1, ' ');

	std::string v_mtlName;

	const std::size_t v_modelSubMeshCount = m_model->m_subMeshData.size();
	for (std::size_t a = 0; a < v_modelSubMeshCount; a++)
	{
		const SubMeshData& v_curSubMesh = m_model->m_subMeshData[a];
	 	const SMTextureList* v_pTexList = m_parent->m_textures->getTexList(v_curSubMesh.m_materialName, a);
		if (!v_pTexList) continue;

		v_mtlName.assign(v_mtlFirstPart);
		String::AppendIntegerToString(v_mtlName, a + 1);
		MaterialManager::AppendMaterialIdx(v_mtlName, v_pTexList->m_material);

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
	const SMTextureList* v_tex_list = m_parent->m_textures->getTexList(name, v_idx);
	if (!v_tex_list) return false;

	return !v_tex_list->m_shadowOnly;
}
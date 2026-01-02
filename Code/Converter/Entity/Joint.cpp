#include "Joint.hpp"

#include "ObjectDatabase/ObjectRotations.hpp"
#include "ObjectDatabase/MaterialManager.hpp"
#include "ObjectDatabase/ObjectData.hpp"

#include "Utils/Console.hpp"

SM_UNMANAGED_CODE

SMJoint::SMJoint(
	const PartData* pParent,
	const glm::vec3& pos,
	Model* pModel,
	const SMColor color,
	const std::uint32_t rotation,
	const std::size_t index
)
	: SMEntityWithModelAndUuid(pParent->m_uuid, pModel, pos)
	, m_parent(pParent)
	, m_index(index)
	, m_color(color)
	, m_xzRotation(rotation)
{}

std::size_t SMJoint::GetIndex() const
{
	return m_index;
}

SMColor SMJoint::GetColor() const
{
	return m_color;
}

EntityType SMJoint::Type() const
{
	return EntityType::Joint;
}

char* SMJoint::GetMtlNameCStr(
	const std::string_view& material,
	const std::size_t idx,
	char* pCString) const
{
	pCString = m_uuid.toCString(pCString);
	*pCString++ = ' ';
	pCString = m_color.StringHexCStr(pCString);
	*pCString++ = ' ';
	pCString = String::FromInteger<std::size_t>(idx + 1, pCString);
	*pCString++ = ' ';

	const SMTextureList* v_tex_data = m_parent->m_textures->getTexList(material, idx);
	if (v_tex_data)
		return MaterialManager::GetMaterialACStr(v_tex_data->m_material, pCString);

	*pCString++ = 'm';
	*pCString++ = '1';

	return pCString;
}

void SMJoint::FillTextureMap(EntityTextureMap& textureMap) const
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

		if (textureMap.find(v_mtlName) != textureMap.end())
			continue;

		textureMap.emplace(
			std::move(v_mtlName),
			ObjectTexDataConstructInfo(*v_pTexList, m_color)
		);
	}
}

glm::mat4 SMJoint::GetTransformMatrix() const
{
	const glm::mat4 v_jointRotation = Rotations::GetRotationMatrix(m_xzRotation);
	const glm::vec3 v_positionOffset = Rotations::GetOffsetPosition(m_xzRotation);

	glm::mat4 v_modelMatrix(1.0f);
	v_modelMatrix *= glm::translate(m_position + v_positionOffset);
	v_modelMatrix *= v_jointRotation;
	v_modelMatrix *= glm::translate(m_parent->m_bounds / 2.0f);

	return v_modelMatrix;
}

bool SMJoint::GetCanWrite(
	const std::string_view& name,
	const std::size_t idx) const
{
	const SMTextureList* v_pTexList = m_parent->m_textures->getTexList(name, idx);
	if (!v_pTexList) return false;

	return !v_pTexList->m_shadowOnly;
}
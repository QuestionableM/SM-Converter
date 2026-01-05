#include "Part.hpp"

#include "ObjectDatabase/ObjectRotations.hpp"
#include "ObjectDatabase/MaterialManager.hpp"

#include "Utils/Console.hpp"

SM_UNMANAGED_CODE

SMPart::SMPart(
	const PartData* pParent,
	const glm::vec3& pos,
	Model* pModel,
	const SMColor color,
	const std::uint8_t rotation,
	const std::size_t index
)
	: SMEntityWithModelAndUuid(pParent->m_uuid, pModel, pos)
	, m_parent(pParent)
	, m_index(index)
	, m_color(color)
	, m_xzRotation(rotation)
{}

std::size_t SMPart::GetIndex() const
{
	return m_index;
}

SMColor SMPart::GetColor() const
{
	return this->m_color;
}

EntityType SMPart::Type() const
{
	return EntityType::Part;
}

char* SMPart::GetMtlNameCStr(
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

	const SMTextureList* v_pTexData = m_parent->m_textures->getTexList(material, idx);
	if (v_pTexData)
		return MaterialManager::GetMaterialACStr(v_pTexData->m_material, pCString);

	*pCString++ = 'm';
	*pCString++ = '1';

	return pCString;
}

void SMPart::FillTextureMap(EntityTextureMap& textureMap) const
{
	std::string v_mtlFirstPart(m_uuid.toString());
	v_mtlFirstPart.append(1, ' ');
	m_color.appendStringHex(v_mtlFirstPart);
	v_mtlFirstPart.append(1, ' ');

	std::string v_mtlName;

	const std::size_t v_subMeshDataCount = m_model->m_subMeshData.size();
	for (std::size_t a = 0; a < v_subMeshDataCount; a++)
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

glm::mat4 SMPart::GetTransformMatrix() const
{
	const glm::mat4 axis_rotation = Rotations::GetRotationMatrix(m_xzRotation);

	glm::mat4 model_matrix(1.0f);
	model_matrix *= glm::translate(m_position);
	model_matrix *= axis_rotation;
	model_matrix *= glm::translate(m_parent->m_bounds / 2.0f);

	return model_matrix;
}

bool SMPart::GetCanWrite(const std::string_view& name, const std::size_t idx) const
{
	const SMTextureList* v_pTexList = m_parent->m_textures->getTexList(name, idx);
	if (!v_pTexList) return false;

	return !v_pTexList->m_shadowOnly;
}
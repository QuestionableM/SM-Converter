#include "Kinematic.hpp"

#include "ObjectDatabase/MaterialManager.hpp"

SM_UNMANAGED_CODE

SMKinematic::SMKinematic(
	const KinematicData* pParent,
	const SMEntityTransform& transform,
	Model* pModel,
	const SMColor color
)
	: SMEntityWithModelAndUuid(pParent->m_uuid, pModel, transform)
	, m_parent(pParent)
	, m_color(color)
{}

SMColor SMKinematic::GetColor() const
{
	return m_color;
}

EntityType SMKinematic::Type() const
{
	return EntityType::Kinematic;
}

char* SMKinematic::GetMtlNameCStr(
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

	const SMTextureList* v_tex_list = m_parent->m_textures->getTexList(material , idx);
	if (v_tex_list)
		return MaterialManager::GetMaterialACStr(v_tex_list->m_material, pCString);

	*pCString++ = 'm';
	*pCString++ = '1';

	return pCString;
}

void SMKinematic::FillTextureMap(EntityTextureMap& textureMap) const
{
	std::string v_mtlFirstPart(m_uuid.toString());
	v_mtlFirstPart.append(1, ' ');
	m_color.appendStringHex(v_mtlFirstPart);
	v_mtlFirstPart.append(1, ' ');

	std::string v_matName;

	const std::size_t v_modelSubMeshCount = m_model->m_subMeshData.size();
	for (std::size_t a = 0; a < v_modelSubMeshCount; a++)
	{
		const SubMeshData& v_curSubMesh = m_model->m_subMeshData[a];
		const SMTextureList* v_pTexList = m_parent->m_textures->getTexList(v_curSubMesh.m_materialName, a);
		if (!v_pTexList) continue;

		v_matName.assign(v_mtlFirstPart);
		String::AppendIntegerToString(v_matName, a + 1);
		MaterialManager::AppendMaterialIdx(v_matName, v_pTexList->m_material);

		if (textureMap.find(v_matName) != textureMap.end())
			continue;

		textureMap.emplace(
			std::move(v_matName),
			ObjectTexDataConstructInfo(*v_pTexList, m_color)
		);
	}
}

bool SMKinematic::GetCanWrite(const std::string_view& name, const std::size_t idx) const
{
	const SMTextureList* v_tex_list = m_parent->m_textures->getTexList(name, idx);
	if (!v_tex_list) return false;

	return !v_tex_list->m_shadowOnly;
}
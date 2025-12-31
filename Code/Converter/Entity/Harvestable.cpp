#include "Harvestable.hpp"

#include "ObjectDatabase\MaterialManager.hpp"

SM_UNMANAGED_CODE

SMHarvestable::SMHarvestable(
	const HarvestableData* pParent,
	const SMEntityTransform& transform,
	Model* pModel,
	SMColor color
)
	: SMEntityWithModelAndUuid(pParent->m_uuid, pModel, transform)
	, m_parent(pParent)
	, m_color(color)
{}

char* SMHarvestable::GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const
{
	v_ptr = m_uuid.toCString(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = m_color.StringHexCStr(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = String::FromInteger<std::size_t>(v_idx + 1, v_ptr);
	*v_ptr++ = ' ';

	const SMTextureList* v_tex_list = m_parent->m_textures->getTexList(v_mat_name, v_idx);
	if (v_tex_list)
		return MaterialManager::GetMaterialACStr(v_tex_list->m_material, v_ptr);

	*v_ptr++ = 'm';
	*v_ptr++ = '1';

	return v_ptr;
}

void SMHarvestable::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
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

		if (tex_map.contains(v_matName)) continue;

		tex_map.emplace(std::move(v_matName), ObjectTexDataConstructInfo(*v_pTexList, m_color));
	}
}

bool SMHarvestable::GetCanWrite(const std::string& name, std::size_t v_idx) const
{
	const SMTextureList* v_tex_list = m_parent->m_textures->getTexList(name, v_idx);
	if (!v_tex_list) return false;

	return !v_tex_list->m_shadowOnly;
}
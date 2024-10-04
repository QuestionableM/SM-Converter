#include "Harvestable.hpp"

#include "ObjectDatabase\MaterialManager.hpp"

#pragma unmanaged

char* SMHarvestable::GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const
{
	v_ptr = m_uuid.ToCString(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = m_color.StringHexCStr(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = String::FromInteger<std::size_t>(v_idx + 1, v_ptr);
	*v_ptr++ = ' ';

	const SMTextureList* v_tex_list = m_parent->m_textures->GetTexList(v_mat_name, v_idx);
	if (v_tex_list)
		return MaterialManager::GetMaterialACStr(v_tex_list->material, v_ptr);

	*v_ptr++ = 'm';
	*v_ptr++ = '1';

	return v_ptr;
}

void SMHarvestable::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	const std::string v_mtlFirstPart = m_uuid.ToString() + ' ' + m_color.StringHex() + ' ';

	const std::size_t v_modelSubMeshCount = m_model->m_subMeshData.size();
	for (std::size_t a = 0; a < v_modelSubMeshCount; a++)
	{
		const SubMeshData& v_curSubMesh = m_model->m_subMeshData[a];
		const SMTextureList* v_pTexList = m_parent->m_textures->GetTexList(v_curSubMesh.m_materialName, a);
		if (!v_pTexList) continue;

		const std::string v_matIdx = MaterialManager::GetMaterialA(v_pTexList->material);
		std::string v_matName = v_mtlFirstPart + std::to_string(a + 1) + " " + v_matIdx;

		if (tex_map.find(v_matName) != tex_map.end())
			continue;

		tex_map.emplace(std::move(v_matName), ObjectTexDataConstructInfo(*v_pTexList, m_color));
	}
}

bool SMHarvestable::GetCanWrite(const std::string& name, std::size_t v_idx) const
{
	const SMTextureList* v_tex_list = m_parent->m_textures->GetTexList(name, v_idx);
	if (!v_tex_list) return false;

	return !v_tex_list->is_shadow_only;
}
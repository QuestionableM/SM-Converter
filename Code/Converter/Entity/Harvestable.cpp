#include "Harvestable.hpp"

#include "ObjectDatabase\MaterialManager.hpp"

#pragma unmanaged

std::string SMHarvestable::GetMtlName(const std::string& mat_name, const std::size_t& mIdx) const
{
	const SubMeshData* pSubMesh = m_model->subMeshData[mIdx];

	std::string material_idx = "m1";

	const SMTextureList* v_tex_list = m_parent->m_textures->GetTexList(pSubMesh->m_MaterialName, mIdx);
	if (v_tex_list)
		material_idx = MaterialManager::GetMaterialA(v_tex_list->material);

	return m_uuid.ToString() + " " + m_color.StringHex() + " " + std::to_string(mIdx + 1) + " " + material_idx;
}

void SMHarvestable::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	const std::string mtl_first_part = m_uuid.ToString() + " " + m_color.StringHex() + " ";
	for (std::size_t a = 0; a < m_model->subMeshData.size(); a++)
	{
		const SubMeshData* pSubMesh = m_model->subMeshData[a];
		const SMTextureList* v_tex_list = m_parent->m_textures->GetTexList(pSubMesh->m_MaterialName, a);
		if (!v_tex_list) continue;

		const std::string v_mat_idx = MaterialManager::GetMaterialA(v_tex_list->material);
		const std::string v_mat_name = mtl_first_part + std::to_string(a + 1) + " " + v_mat_idx;

		if (tex_map.find(v_mat_name) != tex_map.end())
			continue;

		ObjectTexData v_obj_tex_data;
		v_obj_tex_data.m_textures = *v_tex_list;
		v_obj_tex_data.m_tex_color = m_color;

		tex_map.insert(std::make_pair(v_mat_name, v_obj_tex_data));
	}
}

bool SMHarvestable::GetCanWrite(const std::string& name, const std::size_t& v_idx) const
{
	const SMTextureList* v_tex_list = m_parent->m_textures->GetTexList(name, v_idx);
	if (!v_tex_list) return false;

	return !v_tex_list->is_shadow_only;
}
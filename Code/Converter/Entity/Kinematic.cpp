#include "Kinematic.hpp"

#include "ObjectDatabase\MaterialManager.hpp"

#pragma unmanaged

char* SMKinematic::GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const
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

void SMKinematic::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	const std::string v_mtl_first_part = m_uuid.ToString() + ' ' + m_color.StringHex() + ' ';
	for (std::size_t a = 0; a < m_model->subMeshData.size(); a++)
	{
		const SubMeshData* v_sub_mesh = m_model->subMeshData[a];
		const SMTextureList* v_tex_list = m_parent->m_textures->GetTexList(v_sub_mesh->m_MaterialName, a);
		if (!v_tex_list) continue;

		const std::string v_mat_idx = MaterialManager::GetMaterialA(v_tex_list->material);
		const std::string v_mat_name = v_mtl_first_part + std::to_string(a + 1) + ' ' + v_mat_idx;

		if (tex_map.find(v_mat_name) != tex_map.end())
			continue;

		ObjectTexData v_obj_tex_data;
		v_obj_tex_data.m_textures = *v_tex_list;
		v_obj_tex_data.m_tex_color = m_color;

		tex_map.insert(std::make_pair(v_mat_name, v_obj_tex_data));
	}
}

bool SMKinematic::GetCanWrite(const std::string& name, std::size_t v_idx) const
{
	const SMTextureList* v_tex_list = m_parent->m_textures->GetTexList(name, v_idx);
	if (!v_tex_list) return false;

	return !v_tex_list->is_shadow_only;
}
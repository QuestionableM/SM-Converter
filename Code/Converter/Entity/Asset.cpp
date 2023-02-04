#include "Asset.hpp"

#include "ObjectDatabase\MaterialManager.hpp"
#include "ObjectDatabase\ModelStorage.hpp"
#include "ObjectDatabase\ObjectData.hpp"

#pragma unmanaged

SMColor SMAsset::GetColor(const std::string& color) const
{
	{
		const ColorMap::const_iterator v_iter = m_colors.find(color);
		if (v_iter != m_colors.end())
			return v_iter->second;
	}

	{
		const ColorMap::const_iterator v_iter = m_parent->m_defaultColors.find(color);
		if (v_iter != m_parent->m_defaultColors.end())
			return v_iter->second;
	}

	return SMColor(static_cast<unsigned int>(0xffffff));
}

char* SMAsset::GetMtlNameCStr(const std::string& v_mat_name, const std::size_t& v_idx, char* v_ptr) const
{
	v_ptr = m_uuid.ToCString(v_ptr);
	*v_ptr++ = ' ';

	const SMTextureList* v_tex_list = m_parent->m_textures->GetTexList(v_mat_name, v_idx);
	if (v_tex_list)
	{
		v_ptr = this->GetColor(v_tex_list->def_color_idx).StringHexCStr(v_ptr);
		*v_ptr++ = ' ';
		v_ptr = String::FromInteger<std::size_t>(v_idx + 1, v_ptr);
		*v_ptr++ = ' ';
		return MaterialManager::GetMaterialACStr(v_tex_list->material, v_ptr);
	}

	v_ptr = SMColor::WriteEmptyHex(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = String::FromInteger<std::size_t>(v_idx + 1, v_ptr);
	*v_ptr++ = ' ';
	*v_ptr++ = 'm';
	*v_ptr++ = '1';

	return v_ptr;
}

void SMAsset::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	const std::string mtl_first_part = m_uuid.ToString() + " ";
	for (std::size_t a = 0; a < m_model->subMeshData.size(); a++)
	{
		const SubMeshData* pSubMesh = m_model->subMeshData[a];
		const SMTextureList* v_tex_list = m_parent->m_textures->GetTexList(pSubMesh->m_MaterialName, a);
		if (!v_tex_list) continue;

		const SMColor v_color = this->GetColor(v_tex_list->def_color_idx);
		const std::string v_mat_idx = MaterialManager::GetMaterialA(v_tex_list->material);
		const std::string v_mtl_name = mtl_first_part + v_color.StringHex() + " " + std::to_string(a + 1) + " " + v_mat_idx;

		if (tex_map.find(v_mtl_name) != tex_map.end())
			continue;

		ObjectTexData v_obj_tex_data;
		v_obj_tex_data.m_textures = *v_tex_list;
		v_obj_tex_data.m_tex_color = v_color;

		tex_map.insert(std::make_pair(v_mtl_name, v_obj_tex_data));
	}
}

bool SMAsset::GetCanWrite(const std::string& name, const std::size_t& v_idx) const
{
	const SMTextureList* v_tex_list = m_parent->m_textures->GetTexList(name, v_idx);
	if (!v_tex_list) return false;

	return !v_tex_list->is_shadow_only;
}
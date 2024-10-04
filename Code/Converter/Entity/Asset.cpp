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

char* SMAsset::GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const
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
	const std::string v_mtlFirstPart = m_uuid.ToString() + " ";

	const std::size_t v_modelSubMeshCount = m_model->m_subMeshData.size();
	for (std::size_t a = 0; a < v_modelSubMeshCount; a++)
	{
		const SubMeshData& v_curSubMesh = m_model->m_subMeshData[a];
		const SMTextureList* v_pTexList = m_parent->m_textures->GetTexList(v_curSubMesh.m_materialName, a);
		if (!v_pTexList) continue;

		const SMColor v_color = this->GetColor(v_pTexList->def_color_idx);
		const std::string v_matIdx = MaterialManager::GetMaterialA(v_pTexList->material);
		std::string v_mtlName = v_mtlFirstPart + v_color.StringHex() + " " + std::to_string(a + 1) + " " + v_matIdx;

		if (tex_map.find(v_mtlName) != tex_map.end())
			continue;

		tex_map.emplace(std::move(v_mtlName), ObjectTexDataConstructInfo(*v_pTexList, v_color));
	}
}

bool SMAsset::GetCanWrite(const std::string& name, std::size_t v_idx) const
{
	const SMTextureList* v_tex_list = m_parent->m_textures->GetTexList(name, v_idx);
	if (!v_tex_list) return false;

	return !v_tex_list->is_shadow_only;
}
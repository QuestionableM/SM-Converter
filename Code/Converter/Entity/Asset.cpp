#include "Asset.hpp"

#include "ObjectDatabase\MaterialManager.hpp"
#include "ObjectDatabase\ModelStorage.hpp"
#include "ObjectDatabase\ObjectData.hpp"

SM_UNMANAGED_CODE

SMAsset::SMAsset(
	const AssetData* pParent,
	const SMEntityTransform& transform,
	Model* pModel,
	ColorMap&& color_map
)
	: SMEntityWithModelAndUuid(pParent->m_uuid, pModel, transform)
	, m_parent(pParent)
	, m_colors(std::move(color_map))
{}

SMColor SMAsset::GetColor(const std::string& color) const
{
	const Hashing::PrecomputedStringHash v_hash(color);

	{
		const auto v_iter = m_colors.find(v_hash);
		if (v_iter != m_colors.end()) return v_iter->second;
	}

	{
		const auto v_iter = m_parent->m_defaultColors.find(v_hash);
		if (v_iter != m_parent->m_defaultColors.end()) return v_iter->second;
	}

	return SMColor(static_cast<unsigned int>(0xffffff));
}

char* SMAsset::GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const
{
	v_ptr = m_uuid.toCString(v_ptr);
	*v_ptr++ = ' ';

	const SMTextureList* v_tex_list = m_parent->m_textures->getTexList(v_mat_name, v_idx);
	if (v_tex_list)
	{
		v_ptr = this->GetColor(v_tex_list->m_defColorIdx).StringHexCStr(v_ptr);
		*v_ptr++ = ' ';
		v_ptr = String::FromInteger<std::size_t>(v_idx + 1, v_ptr);
		*v_ptr++ = ' ';
		return MaterialManager::GetMaterialACStr(v_tex_list->m_material, v_ptr);
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
	std::string v_mtlFirstPart(m_uuid.toString());
	v_mtlFirstPart.append(1, ' ');

	std::string v_mtlName;

	const std::size_t v_modelSubMeshCount = m_model->m_subMeshData.size();
	for (std::size_t a = 0; a < v_modelSubMeshCount; a++)
	{
		const SubMeshData& v_curSubMesh = m_model->m_subMeshData[a];
		const SMTextureList* v_pTexList = m_parent->m_textures->getTexList(v_curSubMesh.m_materialName, a);
		if (!v_pTexList) continue;

		const SMColor v_color = this->GetColor(v_pTexList->m_defColorIdx);

		v_mtlName.assign(v_mtlFirstPart);
		v_color.appendStringHex(v_mtlName);
		v_mtlName.append(1, ' ');
		String::AppendIntegerToString(v_mtlName, a + 1);
		MaterialManager::AppendMaterialIdx(v_mtlName, v_pTexList->m_material);

		if (tex_map.find(v_mtlName) != tex_map.end())
			continue;

		tex_map.emplace(std::move(v_mtlName), ObjectTexDataConstructInfo(*v_pTexList, v_color));
	}
}

bool SMAsset::GetCanWrite(const std::string& name, std::size_t v_idx) const
{
	const SMTextureList* v_tex_list = m_parent->m_textures->getTexList(name, v_idx);
	if (!v_tex_list) return false;

	return !v_tex_list->m_shadowOnly;
}
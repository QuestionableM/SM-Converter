#include "Asset.hpp"

#include "ObjectDatabase/MaterialManager.hpp"
#include "ObjectDatabase/ModelStorage.hpp"
#include "ObjectDatabase/ObjectData.hpp"

SM_UNMANAGED_CODE

SMAsset::SMAsset(
	const AssetData* pParent,
	const SMEntityTransform& transform,
	Model* pModel,
	ColorMap&& colorMap
)
	: SMEntityWithModelAndUuid(pParent->m_uuid, pModel, transform)
	, m_parent(pParent)
	, m_colors(std::move(colorMap))
{}

SMColor SMAsset::GetColor(const std::string_view& color) const
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

	return SMColor(static_cast<std::uint32_t>(0xffffff));
}

char* SMAsset::GetMtlNameCStr(
	const std::string_view& material,
	const std::size_t idx,
	char* pCString) const
{
	pCString = m_uuid.toCString(pCString);
	*pCString++ = ' ';

	const SMTextureList* v_tex_list = m_parent->m_textures->getTexList(material, idx);
	if (v_tex_list)
	{
		pCString = this->GetColor(v_tex_list->m_defColorIdx).StringHexCStr(pCString);
		*pCString++ = ' ';
		pCString = String::FromInteger<std::size_t>(idx + 1, pCString);
		*pCString++ = ' ';
		return MaterialManager::GetMaterialACStr(v_tex_list->m_material, pCString);
	}

	pCString = SMColor::WriteEmptyHex(pCString);
	*pCString++ = ' ';
	pCString = String::FromInteger<std::size_t>(idx + 1, pCString);
	*pCString++ = ' ';
	*pCString++ = 'm';
	*pCString++ = '1';

	return pCString;
}

void SMAsset::FillTextureMap(EntityTextureMap& textureMap) const
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

		if (textureMap.find(v_mtlName) != textureMap.end())
			continue;

		textureMap.emplace(
			std::move(v_mtlName),
			ObjectTexDataConstructInfo(*v_pTexList, v_color)
		);
	}
}

bool SMAsset::GetCanWrite(const std::string_view& name, const std::size_t idx) const
{
	const SMTextureList* v_pTexList = m_parent->m_textures->getTexList(name, idx);
	if (!v_pTexList) return false;

	return !v_pTexList->m_shadowOnly;
}
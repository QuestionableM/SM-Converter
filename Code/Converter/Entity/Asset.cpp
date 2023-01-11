#include "Asset.hpp"

#include "ObjectDatabase\MaterialManager.hpp"
#include "ObjectDatabase\ModelStorage.hpp"
#include "ObjectDatabase\ObjectData.hpp"

SMColor Asset::GetColor(const std::wstring& color) const
{
	{
		const ColorMap::const_iterator v_iter = m_colors.find(color);
		if (v_iter != m_colors.end())
			return v_iter->second;
	}

	{
		const ColorMap::const_iterator v_iter = m_parent->DefaultColors.find(color);
		if (v_iter != m_parent->DefaultColors.end())
			return v_iter->second;
	}

	return SMColor(static_cast<unsigned int>(0x000000));
}

std::string Asset::GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const
{
	const std::wstring v_name = (m_parent->Textures.Type() == TextureDataType::SubMeshList ? std::to_wstring(mIdx) : mat_name);

	std::string mat_idx = "m1";
	SMColor v_color;

	TextureList tList;
	if (m_parent->Textures.GetEntry(v_name, tList))
	{
		v_color = this->GetColor(tList.def_color_idx);
		mat_idx = MaterialManager::GetMaterialA(tList.material);
	}

	return m_uuid.ToString() + " " + v_color.StringHex() + " " + std::to_string(mIdx + 1) + " " + mat_idx;
}

void Asset::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	const std::string mtl_first_part = m_uuid.ToString() + " ";
	for (std::size_t a = 0; a < m_model->subMeshData.size(); a++)
	{
		const SubMeshData* pSubMesh = m_model->subMeshData[a];
		const std::wstring tex_name = (m_parent->Textures.Type() == TextureDataType::SubMeshList ? std::to_wstring(a) : pSubMesh->m_MaterialName);

		ObjectTexData oTexData;
		if (m_parent->Textures.GetEntry(tex_name, oTexData.Textures))
		{
			oTexData.TexColor = this->GetColor(oTexData.Textures.def_color_idx);

			const std::string mat_idx = MaterialManager::GetMaterialA(oTexData.Textures.material);
			const std::string mtl_name = mtl_first_part + oTexData.TexColor.StringHex() + " " + std::to_string(a + 1) + " " + mat_idx;

			if (tex_map.find(mtl_name) != tex_map.end())
				continue;

			tex_map.insert(std::make_pair(mtl_name, oTexData));
		}
	}
}
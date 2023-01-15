#include "TileClutter.hpp"

#include "ObjectDatabase\MaterialManager.hpp"

std::string	SMTileClutter::GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const
{
	const std::string tex_mat = MaterialManager::GetMaterialA(m_parent->Textures.material);

	return m_uuid.ToString() + " " + m_color.StringHex() + " " + std::to_string(mIdx + 1) + " " + tex_mat;
}

void SMTileClutter::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	const std::string mtl_name = this->GetMtlName(L"", 0);

	if (tex_map.find(mtl_name) != tex_map.end())
		return;

	ObjectTexData oTexData;
	oTexData.Textures = m_parent->Textures;
	oTexData.TexColor = this->m_color;

	tex_map.insert(std::make_pair(mtl_name, oTexData));
}
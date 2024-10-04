#include "TileClutter.hpp"

#include "ObjectDatabase\MaterialManager.hpp"

#pragma unmanaged

char* SMTileClutter::GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const
{
	v_ptr = m_parent->m_uuid.ToCString(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = m_color.StringHexCStr(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = String::FromInteger<std::size_t>(v_idx + 1, v_ptr);
	*v_ptr++ = ' ';

	return MaterialManager::GetMaterialACStr(m_parent->m_textures.material, v_ptr);
}

std::string	SMTileClutter::GetMtlName(std::size_t mIdx) const
{
	const std::string tex_mat = MaterialManager::GetMaterialA(m_parent->m_textures.material);

	return m_parent->m_uuid.ToString() + " " + m_color.StringHex() + " " + std::to_string(mIdx + 1) + " " + tex_mat;
}

void SMTileClutter::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	std::string v_mtlName = this->GetMtlName(0);
	if (tex_map.find(v_mtlName) != tex_map.end())
		return;

	tex_map.emplace(
		std::move(v_mtlName),
		ObjectTexDataConstructInfo(m_parent->m_textures, m_color)
	);
}
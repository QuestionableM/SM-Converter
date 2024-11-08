#include "TileClutter.hpp"

#include "ObjectDatabase\MaterialManager.hpp"

#pragma unmanaged

char* SMTileClutter::GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const
{
	v_ptr = m_parent->m_uuid.toCString(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = m_color.StringHexCStr(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = String::FromInteger<std::size_t>(v_idx + 1, v_ptr);
	*v_ptr++ = ' ';

	return MaterialManager::GetMaterialACStr(m_parent->m_textures.m_material, v_ptr);
}

std::string	SMTileClutter::GetMtlName(std::size_t mIdx) const
{
	std::string v_mtlName(m_parent->m_uuid.toString());
	v_mtlName.append(1, ' ');
	m_color.appendStringHex(v_mtlName);
	v_mtlName.append(1, ' ');
	String::AppendIntegerToString(v_mtlName, mIdx + 1);
	MaterialManager::AppendMaterialIdx(v_mtlName, m_parent->m_textures.m_material);

	return v_mtlName;
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
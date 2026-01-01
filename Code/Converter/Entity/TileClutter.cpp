#include "TileClutter.hpp"

#include "ObjectDatabase\MaterialManager.hpp"

SM_UNMANAGED_CODE

SMTileClutter::SMTileClutter(ClutterData* pParent, Model* pModel)
	: SMEntityWithModel(pModel)
	, m_parent(pParent)
{}

float SMTileClutter::ScaleVariance() const noexcept
{
	return m_parent->m_scaleVariance;
}

void SMTileClutter::SetColor(const SMColor color) noexcept
{
	m_color = color;
}

const SMUuid& SMTileClutter::GetUuid() const
{
	return m_parent->m_uuid;
}

EntityType SMTileClutter::Type() const
{
	return EntityType::Clutter;
}

char* SMTileClutter::GetMtlNameCStr(
	const std::string_view& material,
	const std::size_t idx,
	char* pCString) const
{
	pCString = m_parent->m_uuid.toCString(pCString);
	*pCString++ = ' ';
	pCString = m_color.StringHexCStr(pCString);
	*pCString++ = ' ';
	pCString = String::FromInteger<std::size_t>(idx + 1, pCString);
	*pCString++ = ' ';

	return MaterialManager::GetMaterialACStr(m_parent->m_textures.m_material, pCString);
}

std::string	SMTileClutter::GetMtlName(const std::size_t idx) const
{
	std::string v_mtlName(m_parent->m_uuid.toString());
	v_mtlName.append(1, ' ');
	m_color.appendStringHex(v_mtlName);
	v_mtlName.append(1, ' ');
	String::AppendIntegerToString(v_mtlName, idx + 1);
	MaterialManager::AppendMaterialIdx(v_mtlName, m_parent->m_textures.m_material);

	return v_mtlName;
}

void SMTileClutter::FillTextureMap(EntityTextureMap& textureMap) const
{
	std::string v_mtlName = this->GetMtlName(0);
	if (textureMap.find(v_mtlName) != textureMap.end())
		return;

	textureMap.emplace(
		std::move(v_mtlName),
		ObjectTexDataConstructInfo(m_parent->m_textures, m_color)
	);
}
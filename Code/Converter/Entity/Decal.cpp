#include "Decal.hpp"

#include "ObjectDatabase\MaterialManager.hpp"
#include "ObjectDatabase\ModelStorage.hpp"
#include "ObjectDatabase\ProgCounter.hpp"

#include "Utils\Console.hpp"

SM_UNMANAGED_CODE

SMDecal::SMDecal(
	const DecalData* pData,
	const SMEntityTransform& transform,
	const SMColor color
)
	: SMEntity(transform)
	, m_data(pData)
	, m_color(color)
{}

EntityType SMDecal::Type() const
{
	return EntityType::Decal;
}

char* SMDecal::GetMtlNameCStr(
	const std::string_view& material,
	const std::size_t idx,
	char* pCString) const
{
	pCString = m_data->m_uuid.toCString(pCString);
	*pCString++ = ' ';
	pCString = m_color.StringHexCStr(pCString);
	*pCString++ = ' ';
	pCString = String::FromInteger<std::size_t>(idx + 1, pCString);
	*pCString++ = ' ';

	return MaterialManager::GetMaterialACStr(m_data->m_textures.m_material, pCString);
}

std::string SMDecal::GetMtlName(const std::size_t idx) const
{
	std::string v_mtlName(m_data->m_uuid.toString());
	v_mtlName.append(1, ' ');
	m_color.appendStringHex(v_mtlName);
	v_mtlName.append(1, ' ');
	String::AppendIntegerToString(v_mtlName, idx + 1);
	MaterialManager::AppendMaterialIdx(v_mtlName, m_data->m_textures.m_material);

	return v_mtlName;
}

void SMDecal::FillTextureMap(EntityTextureMap& textureMap) const
{
	std::string v_mtlName = this->GetMtlName(0);
	if (textureMap.find(v_mtlName) != textureMap.end())
		return;

	DebugOutL("Added a new material: ", v_mtlName);

	textureMap.emplace(
		std::move(v_mtlName),
		ObjectTexDataConstructInfo(m_data->m_textures, m_color)
	);
}

void SMDecal::WriteObjectToFile(
	std::ofstream& file,
	WriterOffsetData& offset,
	const glm::mat4& transform) const
{
	const glm::mat4 decal_matrix = transform * this->GetTransformMatrix();

	Model* v_newModel = ModelStorage::LoadModel(L"./Resources/DecalDefault.obj");
	if (v_newModel)
		v_newModel->WriteToFile(decal_matrix, offset, file, this);

	ProgCounter::ProgressValue++;
}
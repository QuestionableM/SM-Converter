#include "Decal.hpp"

#include "ObjectDatabase\MaterialManager.hpp"
#include "ObjectDatabase\ModelStorage.hpp"
#include "ObjectDatabase\ProgCounter.hpp"

#include "Utils\Console.hpp"

#pragma unmanaged

SMDecal::SMDecal(
	const DecalData* data_ptr,
	const SMEntityTransform& transform,
	SMColor color
) :
	SMEntity(transform),
	m_data(data_ptr),
	m_color(color)
{}

EntityType SMDecal::Type() const noexcept
{
	return EntityType::Decal;
}

char* SMDecal::GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const
{
	v_ptr = m_data->m_uuid.toCString(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = m_color.StringHexCStr(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = String::FromInteger<std::size_t>(v_idx + 1, v_ptr);
	*v_ptr++ = ' ';

	return MaterialManager::GetMaterialACStr(m_data->m_textures.m_material, v_ptr);
}

std::string SMDecal::GetMtlName(std::size_t mIdx) const
{
	std::string v_mtlName(m_data->m_uuid.toString());
	v_mtlName.append(1, ' ');
	m_color.appendStringHex(v_mtlName);
	v_mtlName.append(1, ' ');
	String::AppendIntegerToString(v_mtlName, mIdx + 1);
	MaterialManager::AppendMaterialIdx(v_mtlName, m_data->m_textures.m_material);

	return v_mtlName;
}

void SMDecal::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	std::string v_mtlName = this->GetMtlName(0);
	if (tex_map.find(v_mtlName) != tex_map.end())
		return;

	DebugOutL("Added a new material: ", v_mtlName);

	tex_map.emplace(std::move(v_mtlName), ObjectTexDataConstructInfo(m_data->m_textures, m_color));
}

void SMDecal::WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const
{
	const glm::mat4 decal_matrix = transform_matrix * this->GetTransformMatrix();

	Model* v_newModel = ModelStorage::LoadModel(L"./Resources/DecalDefault.obj");
	if (v_newModel)
		v_newModel->WriteToFile(decal_matrix, mOffset, file, this);

	ProgCounter::ProgressValue++;
}
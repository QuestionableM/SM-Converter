#include "Decal.hpp"

#include "ObjectDatabase\MaterialManager.hpp"
#include "ObjectDatabase\ModelStorage.hpp"
#include "ObjectDatabase\ProgCounter.hpp"

#include "Utils\Console.hpp"

#pragma unmanaged

char* SMDecal::GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const
{
	v_ptr = m_data->m_uuid.ToCString(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = m_color.StringHexCStr(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = String::FromInteger<std::size_t>(v_idx + 1, v_ptr);
	*v_ptr++ = ' ';

	return MaterialManager::GetMaterialACStr(m_data->m_textures.material, v_ptr);
}

std::string SMDecal::GetMtlName(std::size_t mIdx) const
{
	const std::string v_materialIdx = MaterialManager::GetMaterialA(m_data->m_textures.material);

	return m_data->m_uuid.ToString() + " " + m_color.StringHex() + " " + std::to_string(mIdx + 1) + " " + v_materialIdx;
}

void SMDecal::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	const std::string v_mtlName = this->GetMtlName(0);
	if (tex_map.find(v_mtlName) != tex_map.end())
		return;

	ObjectTexData v_texData;
	v_texData.m_textures = m_data->m_textures;
	v_texData.m_tex_color = m_color;

	DebugOutL("Added a new material: ", v_mtlName);

	tex_map.insert(std::make_pair(v_mtlName, v_texData));
}

void SMDecal::WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const
{
	const glm::mat4 decal_matrix = transform_matrix * this->GetTransformMatrix();

	Model* v_newModel = ModelStorage::LoadModel(L"./Resources/DecalDefault.obj");
	if (v_newModel)
		v_newModel->WriteToFile(decal_matrix, mOffset, file, this);

	ProgCounter::ProgressValue++;
}
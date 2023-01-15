#include "Decal.hpp"

#include "ObjectDatabase\MaterialManager.hpp"
#include "ObjectDatabase\ModelStorage.hpp"
#include "ObjectDatabase\ProgCounter.hpp"

#include "Utils\Console.hpp"

std::string SMDecal::GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const
{
	const std::string v_materialIdx = MaterialManager::GetMaterialA(m_data->m_textures.material);

	return m_data->m_uuid.ToString() + " " + m_color.StringHex() + " " + std::to_string(mIdx + 1) + " " + v_materialIdx;
}

void SMDecal::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	const std::string v_mtlName = this->GetMtlName(L"", 0);

	if (tex_map.find(v_mtlName) != tex_map.end())
		return;

	ObjectTexData v_texData;
	v_texData.Textures = m_data->m_textures;
	v_texData.TexColor = m_color;

	DebugOutL("Added a new material: ", v_mtlName);

	tex_map.insert(std::make_pair(v_mtlName, v_texData));
}

void SMDecal::WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const
{
	const glm::mat4 decal_matrix = transform_matrix * this->GetTransformMatrix();

	Model* v_newModel = ModelStorage::LoadModel(L"./Resources/test_model.obj");
	v_newModel->WriteToFile(decal_matrix, mOffset, file, this);

	ProgCounter::ProgressValue++;
}
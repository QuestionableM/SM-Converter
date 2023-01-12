#include "Part.hpp"

#include "ObjectDatabase\ObjectRotations.hpp"
#include "ObjectDatabase\MaterialManager.hpp"

#include "Utils\Console.hpp"

#pragma unmanaged

std::string Part::GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const
{
	const SubMeshData* pSubMesh = m_model->subMeshData[mIdx];
	const std::wstring tex_name = (m_parent->Textures.Type() == TextureDataType::SubMeshList ? std::to_wstring(mIdx) : pSubMesh->m_MaterialName);

	std::string material_idx = "m1";

	ObjectTexData oTexData;
	if (m_parent->Textures.GetEntry(tex_name, oTexData.Textures))
		material_idx = MaterialManager::GetMaterialA(oTexData.Textures.material);

	return m_uuid.ToString() + " " + m_color.StringHex() + " " + std::to_string(mIdx + 1) + " " + material_idx;
}

void Part::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	const std::string mtl_first_part = m_uuid.ToString() + " " + m_color.StringHex() + " ";
	for (std::size_t a = 0; a < m_model->subMeshData.size(); a++)
	{
		const SubMeshData* pSubMesh = m_model->subMeshData[a];
		const std::wstring tex_name = (m_parent->Textures.Type() == TextureDataType::SubMeshList ? std::to_wstring(a) : pSubMesh->m_MaterialName);

		ObjectTexData oTexData;
		if (m_parent->Textures.GetEntry(tex_name, oTexData.Textures))
		{
			oTexData.TexColor = m_color;

			const std::string mat_idx = MaterialManager::GetMaterialA(oTexData.Textures.material);
			const std::string mtl_name = mtl_first_part + std::to_string(a + 1) + " " + mat_idx;

			if (tex_map.find(mtl_name) != tex_map.end())
				continue;
			
			tex_map.insert(std::make_pair(mtl_name, oTexData));
		}
	}
}

glm::mat4 Part::GetTransformMatrix() const
{
	const glm::mat4 axis_rotation = Rotations::GetRotationMatrix(m_xAxis, m_zAxis);

	glm::mat4 model_matrix(1.0f);
	model_matrix *= glm::translate(m_position);
	model_matrix *= axis_rotation;
	model_matrix *= glm::translate(m_parent->Bounds / 2.0f);

	return model_matrix;
}
#include "Block.hpp"

#include "ObjectDatabase/ObjectRotations.hpp"
#include "ObjectDatabase/MaterialManager.hpp"
#include "ObjectDatabase/ModelStorage.hpp"
#include "ObjectDatabase/ProgCounter.hpp"

#include "Converter/ConvertSettings.hpp"

#pragma unmanaged

char* SMBlock::GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const
{
	v_ptr = m_uuid.ToCString(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = m_color.StringHexCStr(v_ptr);
	*v_ptr++ = ' ';
	v_ptr = String::FromInteger<std::size_t>(v_idx + 1, v_ptr);
	*v_ptr++ = ' ';

	return MaterialManager::GetMaterialACStr(m_parent->m_textures.material, v_ptr);
}

std::string SMBlock::GetMtlName(std::size_t v_idx) const
{
	const std::string v_mat_name_idx = MaterialManager::GetMaterialA(m_parent->m_textures.material);

	return m_uuid.ToString() + ' ' + m_color.StringHex() + ' ' + std::to_string(v_idx + 1) + ' ' + v_mat_name_idx;
}

void SMBlock::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	const std::string mtl_name = this->GetMtlName(0);
	if (tex_map.find(mtl_name) != tex_map.end())
		return;

	ObjectTexData oTexData;
	oTexData.m_textures = m_parent->m_textures;
	oTexData.m_tex_color = this->m_color;

	tex_map.insert(std::make_pair(mtl_name, oTexData));
}

void GenerateUVs(Model& model, const glm::vec3& bounds, const glm::vec3& pos, int tiling)
{
	model.uvs.resize(24);

	const static glm::vec4 block_texture_normals[6] =
	{
		{  0.0f,  0.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f,  0.0f,  0.0f },
		{  0.0f,  0.0f,  1.0f, -1.0f },
		{  1.0f,  1.0f,  0.0f,  0.0f },
		{  1.0f,  0.0f,  0.0f, -1.0f },
		{ -1.0f,  0.0f,  0.0f, -1.0f }
	};

	const float v_tiling_adj = 1.0f / (float)tiling;
	const glm::vec3 v_offset_pos = pos + bounds;

	for (SubMeshData* v_cur_sub_mesh : model.subMeshData)
	{
		for (auto& v_cur_face : v_cur_sub_mesh->m_DataIdx)
		{
			for (auto& v_cur_vert_data : v_cur_face)
			{
				const glm::vec3 v_block_pos = model.vertices[v_cur_vert_data.m_Vert] + v_offset_pos;

				const glm::vec4& v_cur_norm = block_texture_normals[v_cur_vert_data.m_Norm];
				const glm::vec4 v_uv_data = glm::vec4(v_block_pos.x, v_block_pos.y, v_block_pos.y, v_block_pos.z) * v_cur_norm;

				model.uvs[v_cur_vert_data.m_Uv] = (glm::vec2(v_uv_data.x, v_uv_data.y) + glm::vec2(v_uv_data.z, v_uv_data.w)) * v_tiling_adj;
			}
		}
	}
}

void FillCustomCube(Model& model, const glm::vec3& bounds, const glm::vec3& position, int tiling)
{
	model.vertices =
	{
		{ -bounds.x,  bounds.y,  bounds.z },
		{ -bounds.x, -bounds.y, -bounds.z },
		{ -bounds.x, -bounds.y,  bounds.z },
		{ -bounds.x,  bounds.y, -bounds.z },
		{  bounds.x, -bounds.y, -bounds.z },
		{  bounds.x,  bounds.y, -bounds.z },
		{  bounds.x, -bounds.y,  bounds.z },
		{  bounds.x,  bounds.y,  bounds.z }
	};

	if (SharedConverterSettings::ExportNormals)
	{
		model.normals =
		{
			{ -1.0f,  0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f },
			{  1.0f,  0.0f, 0.0f }, { 0.0f, 0.0f,  1.0f },
			{  0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f,  0.0f }
		};
	}

	SubMeshData* new_subMesh = new SubMeshData(0);

	new_subMesh->has_normals = SharedConverterSettings::ExportNormals;
	new_subMesh->has_uvs     = SharedConverterSettings::ExportUvs;

	new_subMesh->m_DataIdx =
	{
		{ { 0, 0 , 0 }, { 1, 1 , 0 }, { 2, 2 , 0 } },
		{ { 3, 3 , 1 }, { 4, 4 , 1 }, { 1, 5 , 1 } },
		{ { 5, 6 , 2 }, { 6, 7 , 2 }, { 4, 8 , 2 } },
		{ { 7, 9 , 3 }, { 2, 10, 3 }, { 6, 11, 3 } },
		{ { 4, 12, 4 }, { 2, 13, 4 }, { 1, 14, 4 } },
		{ { 3, 15, 5 }, { 7, 16, 5 }, { 5, 17, 5 } },
		{ { 0, 0 , 0 }, { 3, 18, 0 }, { 1, 1 , 0 } },
		{ { 3, 3 , 1 }, { 5, 19, 1 }, { 4, 4 , 1 } },
		{ { 5, 6 , 2 }, { 7, 20, 2 }, { 6, 7 , 2 } },
		{ { 7, 9 , 3 }, { 0, 21, 3 }, { 2, 10, 3 } },
		{ { 4, 12, 4 }, { 6, 22, 4 }, { 2, 13, 4 } },
		{ { 3, 15, 5 }, { 0, 23, 5 }, { 7, 16, 5 } }
	};

	model.subMeshData.push_back(new_subMesh);

	if (SharedConverterSettings::ExportUvs)
		GenerateUVs(model, bounds, position, tiling);
}

void SMBlock::WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const
{
	Model new_block;
	FillCustomCube(new_block, m_size / 2.0f, m_position, m_parent->m_tiling);

	const glm::mat4 block_matrix = transform_matrix * this->GetTransformMatrix();
	new_block.WriteToFile(block_matrix, mOffset, file, this);

	ProgCounter::ProgressValue++;
}

glm::mat4 SMBlock::GetTransformMatrix() const
{
	const glm::mat4 block_matrix = Rotations::GetRotationMatrix(m_xzRotation);

	glm::mat4 transform_matrix(1.0f);
	transform_matrix *= glm::translate(m_position);
	transform_matrix *= block_matrix;
	transform_matrix *= glm::translate(m_size / 2.0f);

	return transform_matrix;
}

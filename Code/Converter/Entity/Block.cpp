#include "Block.hpp"

#include "ObjectDatabase\ObjectRotations.hpp"
#include "ObjectDatabase\MaterialManager.hpp"
#include "ObjectDatabase\ModelStorage.hpp"
#include "ObjectDatabase\ProgCounter.hpp"

#include "Converter\ConvertSettings.hpp"

#pragma unmanaged

std::string SMBlock::GetMtlName(const std::string& mat_name, const std::size_t& mIdx) const
{
	const std::string material_idx = MaterialManager::GetMaterialA(m_parent->m_textures.material);

	return m_uuid.ToString() + ' ' + m_color.StringHex() + ' ' + std::to_string(mIdx + 1) + ' ' + material_idx;
}

void SMBlock::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	const std::string mtl_name = this->GetMtlName("", 0);
	if (tex_map.find(mtl_name) != tex_map.end())
		return;

	ObjectTexData oTexData;
	oTexData.m_textures = m_parent->m_textures;
	oTexData.m_tex_color = this->m_color;

	tex_map.insert(std::make_pair(mtl_name, oTexData));
}

void FillCustomCube(Model& model, const glm::vec3& bounds)
{
	model.vertices = {
		{-bounds.x, bounds.y, bounds.z},
		{-bounds.x, -bounds.y, -bounds.z},
		{-bounds.x, -bounds.y, bounds.z},
		{-bounds.x, bounds.y, -bounds.z},
		{bounds.x, -bounds.y, -bounds.z},
		{bounds.x, bounds.y, -bounds.z},
		{bounds.x, -bounds.y, bounds.z},
		{bounds.x, bounds.y, bounds.z}
	};

	model.uvs = {
		{0.0f, 1.0f}, {1.0f, 0.0f},
		{1.0f, 1.0f}, {1.0f, 1.0f},
		{0.0f, 0.0f}, {1.0f, 0.0f},
		{1.0f, 0.0f}, {0.0f, 1.0f},
		{0.0f, 0.0f}, {1.0f, 1.0f},
		{0.0f, 0.0f}, {1.0f, 0.0f},
		{1.0f, 0.0f}, {0.0f, 1.0f},
		{0.0f, 0.0f}, {1.0f, 0.0f},
		{0.0f, 1.0f}, {0.0f, 0.0f},
		{0.0f, 0.0f}, {0.0f, 1.0f},
		{1.0f, 1.0f}, {0.0f, 1.0f},
		{1.0f, 1.0f}, {1.0f, 1.0f}
	};

	model.normals = {
		{-1.0, 0.0, 0.0}, {0.0, 0.0, -1.0},
		{1.0, 0.0, 0.0}, {0.0, 0.0, 1.0},
		{0.0, -1.0, 0.0}, {0.0, 1.0, 0.0}
	};

	SubMeshData* new_subMesh = new SubMeshData(0);

	new_subMesh->has_normals = SharedConverterSettings::ExportNormals;
	new_subMesh->has_uvs     = SharedConverterSettings::ExportUvs;

	new_subMesh->m_DataIdx = {
		{{0, 0, 0}, {1, 1, 0}, {2, 2, 0}},
		{{3, 3, 1}, {4, 4, 1}, {1, 5, 1}},
		{{5, 6, 2}, {6, 7, 2}, {4, 8, 2}},
		{{7, 9, 3}, {2, 10, 3}, {6, 11, 3}},
		{{4, 12, 4}, {2, 13, 4}, {1, 14, 4}},
		{{3, 15, 5}, {7, 16, 5}, {5, 17, 5}},
		{{0, 0, 0}, {3, 18, 0}, {1, 1, 0}},
		{{3, 3, 1}, {5, 19, 1}, {4, 4, 1}},
		{{5, 6, 2}, {7, 20, 2}, {6, 7, 2}},
		{{7, 9, 3}, {0, 21, 3}, {2, 10, 3}},
		{{4, 12, 4}, {6, 22, 4}, {2, 13, 4}},
		{{3, 15, 5}, {0, 23, 5}, {7, 16, 5}}
	};

	model.subMeshData.push_back(new_subMesh);
}

void AlignUvs(Model& model, const glm::vec3& bounds, const glm::vec3& pos, const int& tiling)
{
	float f_tiling = (float)tiling;

	for (std::size_t a = 0; a < model.uvs.size(); a++)
		model.uvs[a] /= f_tiling;

	glm::vec3 _MulVec = bounds * 2.0f;

	glm::vec2 _PosOff(pos.x / f_tiling + _MulVec.x, pos.y / f_tiling + _MulVec.y);
	(model.uvs[21] *= glm::vec2(0.0f, _MulVec.y)) += _PosOff;
	(model.uvs[11] *= glm::vec2(_MulVec.x, 0.0f)) += _PosOff;
	(model.uvs[9] *= glm::vec2(_MulVec.x, _MulVec.y)) += _PosOff;
	(model.uvs[10] += _PosOff);

	float tile_offset2 = (pos.x + _MulVec.x) / f_tiling * (f_tiling - 1.0f);

	glm::vec2 _Off1(tile_offset2, pos.z / f_tiling + _MulVec.z);
	//2.0f, 0.0f
	(model.uvs[15] *= glm::vec2(_MulVec.x, 0.0f)) += _Off1;
	(model.uvs[16] *= glm::vec2(0.0f, _MulVec.z)) += _Off1;
	(model.uvs[23] *= glm::vec2(_MulVec.x, _MulVec.z)) += _Off1;
	model.uvs[17] += _Off1;

	glm::vec2 _Off2(pos.x / f_tiling + _MulVec.x, pos.z / f_tiling + _MulVec.z);
	//0.0f, 2.0f
	(model.uvs[12] *= glm::vec2(_MulVec.x, 0.0f)) += _Off2;
	(model.uvs[13] *= glm::vec2(0.0f, _MulVec.z)) += _Off2;
	(model.uvs[22] *= glm::vec2(_MulVec.x, _MulVec.z)) += _Off2;
	model.uvs[14] += _Off2;

	glm::vec2 _Off3(tile_offset2, pos.y / f_tiling + _MulVec.y);
	//2.0f, 2.0f
	(model.uvs[5] *= glm::vec2(_MulVec.x, 0.0f)) += _Off3;
	(model.uvs[19] *= glm::vec2(0.0f, _MulVec.y)) += _Off3;
	(model.uvs[3] *= glm::vec2(_MulVec.x, _MulVec.y)) += _Off3;
	model.uvs[4] += _Off3;

	glm::vec2 _Off4(
		(pos.y + _MulVec.y) / f_tiling * (f_tiling - 1.0f),
		pos.z / f_tiling + _MulVec.z
	);
	//4.0f, 2.0f
	(model.uvs[1] *= glm::vec2(_MulVec.y, 0.0f)) += _Off4;
	(model.uvs[0] *= glm::vec2(0.0f, _MulVec.z)) += _Off4;
	(model.uvs[2] *= glm::vec2(_MulVec.y, _MulVec.z)) += _Off4;
	model.uvs[18] += _Off4;

	glm::vec2 _Off5(pos.y / f_tiling + _MulVec.y, pos.z / f_tiling + _MulVec.z);
	(model.uvs[6] *= glm::vec2(_MulVec.y, 0.0f)) += _Off5;
	(model.uvs[7] *= glm::vec2(0.0f, _MulVec.z)) += _Off5;
	(model.uvs[20] *= glm::vec2(_MulVec.y, _MulVec.z)) += _Off5;
	model.uvs[8] += _Off5;
}

void SMBlock::WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const
{
	const glm::mat4 block_matrix = transform_matrix * this->GetTransformMatrix();

	Model new_block;
	FillCustomCube(new_block, m_size / 2.0f);
	AlignUvs(new_block, m_size / 2.0f, m_position, m_parent->m_tiling);

	new_block.WriteToFile(block_matrix, mOffset, file, this);

	ProgCounter::ProgressValue++;
}

glm::mat4 SMBlock::GetTransformMatrix() const
{
	const glm::mat4 block_matrix = Rotations::GetRotationMatrix(m_xAxis, m_zAxis);

	glm::mat4 transform_matrix(1.0f);
	transform_matrix *= glm::translate(m_position);
	transform_matrix *= block_matrix;
	transform_matrix *= glm::translate(m_size / 2.0f);

	return transform_matrix;
}
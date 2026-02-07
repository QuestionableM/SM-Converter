#include "Block.hpp"

#include "ObjectDatabase/ObjectRotations.hpp"
#include "ObjectDatabase/MaterialManager.hpp"
#include "ObjectDatabase/ModelStorage.hpp"
#include "ObjectDatabase/ProgCounter.hpp"

#include "Converter/ConvertSettings.hpp"

SM_UNMANAGED_CODE

SMBlock::SMBlock(
	SMBlock* other,
	const glm::vec3& pos,
	const glm::quat& rot,
	const glm::vec3& scale
)
	: SMEntityWithUuid(other->m_parent->m_uuid, pos, scale)
	, m_parent(other->m_parent)
	, m_index(0)
	, m_color(other->m_color)
	, m_xzRotation(other->m_xzRotation)
{}

SMBlock::SMBlock(
	const BlockData* pParent,
	const glm::vec3& pos,
	const glm::vec3& scale,
	const SMColor color,
	const std::uint8_t rotation,
	const std::size_t index
)
	: SMEntityWithUuid(pParent->m_uuid, pos, scale)
	, m_parent(pParent)
	, m_index(index)
	, m_color(color)
	, m_xzRotation(rotation)
{}

std::size_t SMBlock::GetIndex() const
{
	return m_index;
}

SMColor SMBlock::GetColor() const
{
	return m_color;
}

EntityType SMBlock::Type() const
{
	return EntityType::Block;
}

char* SMBlock::GetMtlNameCStr(
	const std::string_view& material,
	const std::size_t idx,
	char* pCString) const
{
	pCString = m_uuid.toCString(pCString);
	*pCString++ = ' ';
	pCString = m_color.StringHexCStr(pCString);
	*pCString++ = ' ';
	pCString = String::FromInteger<std::size_t>(idx + 1, pCString);
	*pCString++ = ' ';

	return MaterialManager::GetMaterialACStr(m_parent->m_textures.m_material, pCString);
}

std::string SMBlock::GetMtlName(const std::size_t idx) const
{
	std::string v_mtlName(m_uuid.toString());
	v_mtlName.append(1, ' ');
	m_color.appendStringHex(v_mtlName);
	v_mtlName.append(1, ' ');
	String::AppendIntegerToString(v_mtlName, idx + 1);
	MaterialManager::AppendMaterialIdx(v_mtlName, m_parent->m_textures.m_material);

	return v_mtlName;
}

void SMBlock::FillTextureMap(
	EntityTextureMap& textureMap) const
{
	std::string v_mtlName = this->GetMtlName(0);
	if (textureMap.find(v_mtlName) != textureMap.end())
		return;

	textureMap.emplace(
		std::move(v_mtlName),
		ObjectTexDataConstructInfo(m_parent->m_textures, m_color)
	);
}

static void GenerateUVs(Model& model, const glm::vec3& bounds, const glm::vec3& pos, int tiling)
{
	model.m_uvs.resize(24);

	const static glm::vec4 v_blockTextureNormals[6] =
	{
		{  0.0f,  0.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f,  0.0f,  0.0f },
		{  0.0f,  0.0f,  1.0f, -1.0f },
		{  1.0f,  1.0f,  0.0f,  0.0f },
		{  1.0f,  0.0f,  0.0f, -1.0f },
		{ -1.0f,  0.0f,  0.0f, -1.0f }
	};

	const float v_tilingAdj = 1.0f / (float)tiling;
	const glm::vec3 v_offsetPos = pos + bounds;

	for (const SubMeshData& v_curSubMesh : model.m_subMeshData)
	{
		for (const auto& v_curFace : v_curSubMesh.m_dataIdx)
		{
			for (const VertexData& v_curVertData : v_curFace)
			{
				const glm::vec3 v_vertPos = model.m_vertices[v_curVertData.m_vert] + v_offsetPos;

				const glm::vec4& v_curNorm = v_blockTextureNormals[v_curVertData.m_norm];
				const glm::vec4 v_uvData = glm::vec4(v_vertPos.x, v_vertPos.y, v_vertPos.y, v_vertPos.z) * v_curNorm;

				model.m_uvs[v_curVertData.m_uv] = (glm::vec2(v_uvData.x, v_uvData.y) + glm::vec2(v_uvData.z, v_uvData.w)) * v_tilingAdj;
			}
		}
	}
}

static void FillCustomCube(Model& model, const glm::vec3& bounds, const glm::vec3& position, int tiling)
{
	model.m_vertices =
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
		model.m_normals =
		{
			{ -1.0f,  0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f },
			{  1.0f,  0.0f, 0.0f }, { 0.0f, 0.0f,  1.0f },
			{  0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f,  0.0f }
		};
	}

	model.m_subMeshData.emplace_back(
		0,
		SharedConverterSettings::ExportNormals,
		SharedConverterSettings::ExportUvs,
		std::initializer_list<std::initializer_list<VertexData>>{
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
		}
	);

	if (SharedConverterSettings::ExportUvs)
		GenerateUVs(model, bounds, position, tiling);
}

void SMBlock::WriteObjectToFile(
	std::ofstream& file,
	WriterOffsetData& offset,
	const glm::mat4& transform) const
{
	Model v_blockModel;
	FillCustomCube(v_blockModel, m_size / 2.0f, m_position, m_parent->m_tiling);

	const glm::mat4 v_blockTransform = transform * this->GetTransformMatrix();
	v_blockModel.WriteToFile(v_blockTransform, offset, file, this);

	ProgCounter::ProgressValue++;
}

void SMBlock::WriteObjectToFileGltf(
	GltfWriterContext& context,
	const glm::mat4& transform) const
{
	DebugOutL("TODO -> SMBlock writer for GLTF");
}

glm::mat4 SMBlock::GetTransformMatrix() const
{
	const glm::mat4 v_blockRotation = Rotations::GetRotationMatrix(m_xzRotation);

	glm::mat4 v_transform(1.0f);
	v_transform *= glm::translate(m_position);
	v_transform *= v_blockRotation;
	v_transform *= glm::translate(m_size / 2.0f);

	return v_transform;
}
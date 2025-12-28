#include "Wedge.hpp"

#include "ObjectDatabase/ObjectRotations.hpp"
#include "ObjectDatabase/MaterialManager.hpp"
#include "ObjectDatabase/ModelStorage.hpp"
#include "ObjectDatabase/ProgCounter.hpp"

#include "Converter/ConvertSettings.hpp"

SM_UNMANAGED_CODE

SMWedge::SMWedge(
	const WedgeData* pParent,
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

char* SMWedge::GetMtlNameCStr(const std::string& matName, std::size_t idx, char* ptr) const
{
	ptr = m_uuid.toCString(ptr);
	*ptr++ = ' ';
	ptr = m_color.StringHexCStr(ptr);
	*ptr++ = ' ';
	ptr = String::FromInteger<std::size_t>(idx + 1, ptr);
	*ptr++ = ' ';

	return MaterialManager::GetMaterialACStr(m_parent->m_parentBlock->m_textures.m_material, ptr);
}

std::string SMWedge::GetMtlName(std::size_t idx) const
{
	std::string v_mtlName(m_uuid.toString());
	v_mtlName.append(1, ' ');
	m_color.appendStringHex(v_mtlName);
	v_mtlName.append(1, ' ');
	String::AppendIntegerToString(v_mtlName, idx + 1);
	MaterialManager::AppendMaterialIdx(v_mtlName, m_parent->m_parentBlock->m_textures.m_material);

	return v_mtlName;
}

void SMWedge::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& outTexMap) const
{
	std::string v_mtlName = this->GetMtlName(0);
	if (outTexMap.find(v_mtlName) != outTexMap.end())
		return;

	outTexMap.emplace(std::move(v_mtlName), ObjectTexDataConstructInfo(m_parent->m_parentBlock->m_textures, m_color));
}

static void GenerateUVs(Model& model, const glm::vec3& bounds, const glm::vec3& position, const int tiling)
{
	model.m_uvs.resize(24);

	const static glm::vec4 v_wedgeTextureNormals[6] =
	{
		{  0.0f,  0.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f,  0.0f,  0.0f },
		{  0.0f,  0.0f,  1.0f, -1.0f },
		{  1.0f,  1.0f,  0.0f,  0.0f },
		{  1.0f,  0.0f,  0.0f, -1.0f },
		{ -1.0f,  0.0f,  0.0f, -1.0f }
	};

	const float v_tilingAdj = 1.0f / static_cast<float>(tiling);
	const glm::vec3 v_offsetPos = bounds + position;

	for (const SubMeshData& v_curSubMesh : model.m_subMeshData)
	{
		for (const auto& v_curFace : v_curSubMesh.m_dataIdx)
		{
			for (const VertexData& v_curVertData : v_curFace)
			{
				const glm::vec3 v_vertPos = model.m_vertices[v_curVertData.m_vert] + v_offsetPos;

				const glm::vec4& v_curNorm = v_wedgeTextureNormals[v_curVertData.m_norm];
				const glm::vec4 v_uvData = glm::vec4(v_vertPos.x, v_vertPos.y, v_vertPos.y, v_vertPos.z) * v_curNorm;

				model.m_uvs[v_curVertData.m_uv] = (glm::vec2(v_uvData.x, v_uvData.y) + glm::vec2(v_uvData.z, v_uvData.w)) * v_tilingAdj;
			}
		}
	}
}

static void FillCustomCube(Model& model, const glm::vec3& bounds, const glm::vec3& position, const int tiling)
{
	model.m_vertices =
	{
		// v -0.500000 -0.500000 -0.500000
		{ -bounds.x, -bounds.y, -bounds.z },
		// v -0.500000 0.500000 -0.500000
		{ -bounds.x, bounds.y, -bounds.z },
		// v 0.500000 -0.500000 -0.500000
		{ bounds.x, -bounds.y, -bounds.z },
		// v 0.500000 0.500000 -0.500000
		{ bounds.x, bounds.y, -bounds.z },
		// v -0.500000 -0.500000 0.500000
		{ -bounds.x, -bounds.y, bounds.z },
		// v 0.500000 -0.500000 0.500000
		{ bounds.x, -bounds.y, bounds.z }
	};

	if (SharedConverterSettings::ExportNormals)
	{
		model.m_normals =
		{
			// vn -0.0000 -0.0000 -1.0000
			{ 0.0f, 0.0f, -1.0f },
			// vn -0.0000 -1.0000 -0.0000
			{ 0.0f, -1.0f, 0.0f },
			// vn -1.0000 -0.0000 -0.0000
			{ -1.0f, 0.0f, 0.0f },
			// vn 1.0000 -0.0000 -0.0000
			{ 1.0f, 0.0f, 0.0f },
			// vn -0.0000 0.7071 0.7071
			{ 0.0f, 0.7071, 0.7071 }
		};
	}

	model.m_subMeshData.emplace_back(
		0,
		SharedConverterSettings::ExportNormals,
		SharedConverterSettings::ExportUvs,
		std::initializer_list<std::initializer_list<VertexData>>{
			// f 2/1/1 3/2/1 1/3/1
			{ { 1, 0, 0 }, { 2, 1, 0 }, { 0, 2, 0 } },
			// f 6/4/2 1/5/2 3/6/2
			{ { 5, 3, 1 }, { 0, 4, 1 }, { 2, 5, 1 } },
			// f 2/1/3 1/5/3 5/7/3
			{ { 1, 6, 2 }, { 0, 7, 2 }, { 4, 8, 2 } },
			// f 3/6/4 4/8/4 6/4/4
			{ { 2, 9, 3 }, { 3, 10, 3 }, { 5, 11, 3 } },
			// f 2/1/5 6/4/5 4/8/5
			{ { 1, 12, 4 }, { 5, 13, 4 }, { 3, 14, 4 } },
			// f 2/1/1 4/8/1 3/2/1
			{ { 1, 15, 0 }, { 3, 16, 0 }, { 2, 17, 0 } },
			// f 6/4/2 5/7/2 1/5/2
			{ { 5, 18, 1 }, { 4, 19, 1 }, { 0, 20, 1 } },
			// f 2/1/5 5/7/5 6/4/5
			{ { 1, 21, 4 }, { 4, 22, 4 }, { 5, 23, 4 } }
			// f 2/1/1 3/2/1 1/3/1
			//{ { 1, 0, 0 }, { 2, 1, 0 }, { 0, 2, 0 } },
			// f 6/4/2 1/5/2 3/6/2
			//{ { 5, 3, 1 }, { 0, 4, 1 }, { 2, 5, 1 } },
			// f 2/1/3 1/5/3 5/7/3
			//{ { 1, 0, 2 }, { 0, 4, 2 }, { 4, 6, 2 } },
			// f 3/6/4 4/8/4 6/4/4
			//{ { 2, 5, 3 }, { 3, 7, 3 }, { 5, 3, 3 } },
			// f 2/1/5 6/4/5 4/8/5
			//{ { 1, 0, 4 }, { 5, 3, 4 }, { 3, 7, 4 } },
			// f 2/1/1 4/8/1 3/2/1
			//{ { 1, 0, 0 }, { 3, 7, 0 }, { 2, 1, 0 } },
			// f 6/4/2 5/7/2 1/5/2
			//{ { 5, 3, 1 }, { 4, 6, 1 }, { 0, 4, 1 } },
			// f 2/1/5 5/7/5 6/4/5
			//{ { 1, 0, 4 }, { 4, 6, 4 }, { 5, 3, 4 } }
		}
	);

	if (SharedConverterSettings::ExportUvs)
		GenerateUVs(model, bounds, position, tiling);
}

void SMWedge::WriteObjectToFile(std::ofstream& file, WriterOffsetData& offset, const glm::mat4& transformMatrix) const
{
	Model v_newWedge;
	FillCustomCube(v_newWedge, m_size * 0.5f, m_position, m_parent->m_parentBlock->m_tiling);

	const glm::mat4 v_wedgeMatrix = transformMatrix * this->GetTransformMatrix();
	v_newWedge.WriteToFile(v_wedgeMatrix, offset, file, this);

	ProgCounter::ProgressValue++;
}

glm::mat4 SMWedge::GetTransformMatrix() const
{
	const glm::mat4 v_wedgeMatrix = Rotations::GetRotationMatrix(m_xzRotation);

	glm::mat4 v_transformMatrix(1.0f);
	v_transformMatrix *= glm::translate(m_position);
	v_transformMatrix *= v_wedgeMatrix;
	v_transformMatrix *= glm::translate(m_size / 2.0f);

	return v_transformMatrix;
}
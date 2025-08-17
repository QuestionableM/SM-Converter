#include "ModelStorage.hpp"

#include "Converter\ConvertSettings.hpp"
#include "Converter\Entity\Entity.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"

#pragma unmanaged

static char g_modelWriterBuf[1024];
static char* g_modelWriterBufferEnd = g_modelWriterBuf + sizeof(g_modelWriterBuf);
static char* g_modelWriterPtr;

SubMeshData::SubMeshData(
	const std::uint32_t idx,
	const bool hasNormals,
	const bool hasUvs
)
	: m_materialName()
	, m_dataIdx()
	, m_subMeshIdx(idx)
	, m_hasNormals(hasNormals)
	, m_hasUvs(hasUvs)
{}

SubMeshData::SubMeshData(
	const std::uint32_t idx,
	const bool hasNormals,
	const bool hasUvs,
	const std::initializer_list<std::initializer_list<VertexData>>& dataIdx
)
	: m_materialName()
	, m_dataIdx(dataIdx.begin(), dataIdx.end())
	, m_subMeshIdx(idx)
	, m_hasNormals(hasNormals)
	, m_hasUvs(hasUvs)
{}

SubMeshData::SubMeshData(SubMeshData&& other) noexcept
	: m_materialName(std::move(other.m_materialName))
	, m_dataIdx(std::move(other.m_dataIdx))
	, m_subMeshIdx(other.m_subMeshIdx)
	, m_hasNormals(other.m_hasNormals)
	, m_hasUvs(other.m_hasUvs)
{}

inline static char* WriteVertexOnly(char* ptr, const std::size_t vertIdx)
{
	*ptr++ = ' ';
	ptr = String::FromInteger<std::size_t>(vertIdx, ptr);

	return ptr;
}

inline static char* WriteNormals(char* ptr, const std::size_t vertIdx, const std::size_t normIdx)
{
	*ptr++ = ' ';
	ptr = String::FromInteger<std::size_t>(vertIdx, ptr);
	*ptr++ = '/';
	*ptr++ = '/';
	ptr = String::FromInteger<std::size_t>(normIdx, ptr);

	return ptr;
}

inline static char* WriteUvs(char* ptr, const std::size_t vertIdx, const std::size_t uvIdx)
{
	*ptr++ = ' ';
	ptr = String::FromInteger<std::size_t>(vertIdx, ptr);
	*ptr++ = '/';
	ptr = String::FromInteger<std::size_t>(uvIdx, ptr);

	return ptr;
}

inline static char* WriteUvsAndNormals(
	char* ptr,
	const std::size_t vertIdx,
	const std::size_t uvIdx,
	const std::size_t normIdx)
{
	*ptr++ = ' ';
	ptr = String::FromInteger<std::size_t>(vertIdx, ptr);
	*ptr++ = '/';
	ptr = String::FromInteger<std::size_t>(uvIdx, ptr);
	*ptr++ = '/';
	ptr = String::FromInteger<std::size_t>(normIdx, ptr);

	return ptr;
}

void SubMeshData::IndexWriter_None(const IndexWriterArguments& data, const VertexData& vert)
{
	const glm::vec3& v_vertex = data.m_translatedVertices[vert.m_vert];
	const std::size_t v_vertIdx = data.m_offset.VertexMap.at(v_vertex);

	g_modelWriterPtr = WriteVertexOnly(g_modelWriterPtr, v_vertIdx);
}

void SubMeshData::IndexWriter_NoneUnoptimized(const IndexWriterArguments& data, const VertexData& vert)
{
	const std::size_t v_vertIdx = vert.m_vert + data.m_offset.Vertex;
	g_modelWriterPtr = WriteVertexOnly(g_modelWriterPtr, v_vertIdx);
}

void SubMeshData::IndexWriter_Normals(const IndexWriterArguments& data, const VertexData& vert)
{
	const glm::vec3& v_vertex = data.m_translatedVertices[vert.m_vert];
	const glm::vec3& v_normal = data.m_translatedNormals[vert.m_norm];

	const std::size_t v_vertIdx = data.m_offset.VertexMap.at(v_vertex);
	const std::size_t v_normIdx = data.m_offset.NormalMap.at(v_normal);

	g_modelWriterPtr = WriteNormals(g_modelWriterPtr, v_vertIdx, v_normIdx);
}

void SubMeshData::IndexWriter_NormalsUnoptimized(const IndexWriterArguments& data, const VertexData& vert)
{
	const glm::vec3& v_normal = data.m_translatedNormals[vert.m_norm];

	const std::size_t v_vertIdx = vert.m_vert + data.m_offset.Vertex;
	const std::size_t v_normIdx = data.m_offset.NormalMap.at(v_normal);

	g_modelWriterPtr = WriteNormals(g_modelWriterPtr, v_vertIdx, v_normIdx);
}

void SubMeshData::IndexWriter_Uvs(const IndexWriterArguments& data, const VertexData& vert)
{
	const glm::vec3& v_vertex = data.m_translatedVertices[vert.m_vert];
	const std::size_t v_vertIdx = data.m_offset.VertexMap.at(v_vertex);

	g_modelWriterPtr = WriteUvs(g_modelWriterPtr, v_vertIdx, vert.m_uv);
}

void SubMeshData::IndexWriter_UvsUnoptimized(const IndexWriterArguments& data, const VertexData& vert)
{
	const std::size_t v_vertIdx = vert.m_vert + data.m_offset.Vertex;
	g_modelWriterPtr = WriteUvs(g_modelWriterPtr, v_vertIdx, vert.m_uv);
}

void SubMeshData::IndexWriter_UvsAndNormals(const IndexWriterArguments& data, const VertexData& vert)
{
	const glm::vec3& v_vertex = data.m_translatedVertices[vert.m_vert];
	const glm::vec3& v_normal = data.m_translatedNormals[vert.m_norm];

	const std::size_t v_vertIdx = data.m_offset.VertexMap.at(v_vertex);
	const std::size_t v_normIdx = data.m_offset.NormalMap.at(v_normal);

	g_modelWriterPtr = WriteUvsAndNormals(g_modelWriterPtr, v_vertIdx, vert.m_uv, v_normIdx);
}

void SubMeshData::IndexWriter_UvsAndNormalsUnoptimized(const IndexWriterArguments& data, const VertexData& vert)
{
	const glm::vec3& v_normal = data.m_translatedNormals[vert.m_norm];
	const std::size_t v_normIdx = data.m_offset.NormalMap.at(v_normal);

	const std::size_t v_vertIdx = vert.m_vert + data.m_offset.Vertex;

	g_modelWriterPtr = WriteUvsAndNormals(g_modelWriterPtr, v_vertIdx, vert.m_uv, v_normIdx);
}

SubMeshData::IndexWriterFunction SubMeshData::getWriterFunction() const noexcept
{
	const SubMeshData::IndexWriterFunction v_writerFunctions[8] =
	{
		// Optimized algos

		SubMeshData::IndexWriter_None,
		SubMeshData::IndexWriter_Normals,
		SubMeshData::IndexWriter_Uvs,
		SubMeshData::IndexWriter_UvsAndNormals,

		// Unoptimized algos

		SubMeshData::IndexWriter_NoneUnoptimized,
		SubMeshData::IndexWriter_NormalsUnoptimized,
		SubMeshData::IndexWriter_UvsUnoptimized,
		SubMeshData::IndexWriter_UvsAndNormalsUnoptimized
	};

	const std::size_t v_funcIdx = static_cast<std::size_t>(m_hasNormals) | (static_cast<std::size_t>(m_hasUvs) << 1);
	const std::size_t v_unoptimizedOffset = SharedConverterSettings::DeduplicateVertices ? 0 : 4;

	return v_writerFunctions[v_funcIdx + v_unoptimizedOffset];
}

/////////////////////// MODEL //////////////////////

Model::Model(const std::wstring_view& mesh_path)
	: m_path(mesh_path)
	, m_vertices()
	, m_normals()
	, m_uvs()
	, m_subMeshData()
	, m_bUvsCached(false)
{}

Model::Model()
	: m_path()
	, m_vertices()
	, m_normals()
	, m_uvs()
	, m_subMeshData()
	, m_bUvsCached(false)
{}

// This is actually faster than sprintf("%g %g %g")
inline static char* WriteVec3Line(char* ptr, const glm::vec3& vertex)
{
	ptr = String::FromFloat(vertex.x, ptr);
	*ptr++ = ' ';
	ptr = String::FromFloat(vertex.y, ptr);
	*ptr++ = ' ';
	ptr = String::FromFloat(vertex.z, ptr);
	*ptr++ = '\n';

	return ptr;
}

void Model::WriteToFile(
	const glm::mat4& model_mat,
	WriterOffsetData& offset,
	std::ofstream& file,
	const SMEntity* pEntity)
{
	std::vector<glm::vec3> v_translatedVertices(SharedConverterSettings::DeduplicateVertices ? m_vertices.size() : 0);
	std::vector<glm::vec3> v_translatedNormals(m_normals.size());

	char* v_writer_off_two = g_modelWriterBuf + 2;
	char* v_writer_off_three = g_modelWriterBuf + 3;

	g_modelWriterBuf[0] = 'v';
	g_modelWriterBuf[1] = ' ';

	if (SharedConverterSettings::DeduplicateVertices)
	{
		const std::size_t v_verticesCount = m_vertices.size();
		for (std::size_t a = 0; a < v_verticesCount; a++)
		{
			const glm::vec3 v_vertex = model_mat * glm::vec4(m_vertices[a], 1.0f);

			v_translatedVertices[a] = v_vertex;

			if (offset.VertexMap.emplace(v_vertex, offset.Vertex).second)
			{
				offset.Vertex++;

				g_modelWriterPtr = WriteVec3Line(v_writer_off_two, v_vertex);
				file.write(g_modelWriterBuf, g_modelWriterPtr - g_modelWriterBuf);
			}
		}
	}
	else
	{
		const std::size_t v_numVertices = m_vertices.size();
		for (std::size_t a = 0; a < v_numVertices; a++)
		{
			const glm::vec3 v_vertex = model_mat * glm::vec4(m_vertices[a], 1.0f);

			g_modelWriterPtr = WriteVec3Line(v_writer_off_two, v_vertex);
			file.write(g_modelWriterBuf, g_modelWriterPtr - g_modelWriterBuf);
		}
	}

	if (SharedConverterSettings::ExportUvs && !m_bUvsCached)
	{
		m_bUvsCached = true;

		g_modelWriterBuf[1] = 't';
		g_modelWriterBuf[2] = ' ';

		//Writing the uvs
		const std::size_t v_uvCount = m_uvs.size();
		for (std::size_t a = 0; a < v_uvCount; a++)
		{
			const glm::vec2& v_uv = m_uvs[a];

			if (offset.UvMap.emplace(v_uv, offset.Uv).second)
			{
				offset.Uv++;

				g_modelWriterPtr = String::FromFloat(v_uv.x, v_writer_off_three);
				*g_modelWriterPtr++ = ' ';
				g_modelWriterPtr = String::FromFloat(v_uv.y, g_modelWriterPtr);
				*g_modelWriterPtr++ = '\n';

				file.write(g_modelWriterBuf, g_modelWriterPtr - g_modelWriterBuf);
			}
		}

		//Connecting indexes
		const std::size_t v_subMeshDataCount = m_subMeshData.size();
		for (std::size_t a = 0; a < v_subMeshDataCount; a++)
		{
			SubMeshData& v_curSubMesh = m_subMeshData[a];
			if (!v_curSubMesh.m_hasUvs) continue;

			const std::size_t v_dataIdxCount = v_curSubMesh.m_dataIdx.size();
			for (std::size_t b = 0; b < v_dataIdxCount; b++)
			{
				std::vector<VertexData>& v_vecVertData = v_curSubMesh.m_dataIdx[b];

				const std::size_t v_vertDataCount = v_vecVertData.size();
				for (std::size_t c = 0; c < v_vertDataCount; c++)
				{
					VertexData& v_curVert = v_vecVertData[c];

					const glm::vec2& v_curUv = m_uvs[v_curVert.m_uv];
					v_curVert.m_uv = offset.UvMap.at(v_curUv);
				}
			}
		}
	}

	if (SharedConverterSettings::ExportNormals)
	{
		g_modelWriterBuf[1] = 'n';
		g_modelWriterBuf[2] = ' ';

		const glm::mat3 v_rotMatrix(model_mat);

		const std::size_t v_normalCount = m_normals.size();
		for (std::size_t a = 0; a < v_normalCount; a++)
		{
			const glm::vec3 v_normal = v_rotMatrix * m_normals[a];

			v_translatedNormals[a] = v_normal;

			if (offset.NormalMap.emplace(v_normal, offset.Normal).second)
			{
				offset.Normal++;

				g_modelWriterPtr = WriteVec3Line(v_writer_off_three, v_normal);
				file.write(g_modelWriterBuf, g_modelWriterPtr - g_modelWriterBuf);
			}
		}
	}

	IndexWriterArguments v_idxWriterArgs(this, offset, v_translatedVertices, v_translatedNormals);

	const std::size_t v_subMeshDataCount = m_subMeshData.size();
	for (std::size_t mIdx = 0; mIdx < v_subMeshDataCount; mIdx++)
	{
		const SubMeshData& v_curSubMesh = m_subMeshData[mIdx];

		if (pEntity != nullptr)
		{
			//Skip writing the sub mesh if entity doesn't allow it
			if (!pEntity->GetCanWrite(v_curSubMesh.m_materialName, mIdx))
				continue;

			if (SharedConverterSettings::ExportMaterials)
			{
				g_modelWriterPtr = g_modelWriterBuf;
				*g_modelWriterPtr++ = 'u';
				*g_modelWriterPtr++ = 's';
				*g_modelWriterPtr++ = 'e';
				*g_modelWriterPtr++ = 'm';
				*g_modelWriterPtr++ = 't';
				*g_modelWriterPtr++ = 'l';
				*g_modelWriterPtr++ = ' ';

				g_modelWriterPtr = pEntity->GetMtlNameCStr(v_curSubMesh.m_materialName, mIdx, g_modelWriterPtr);
				*g_modelWriterPtr++ = '\n';

				file.write(g_modelWriterBuf, g_modelWriterPtr - g_modelWriterBuf);
			}
		}

		v_idxWriterArgs.m_subMesh = &v_curSubMesh;

		SubMeshData::IndexWriterFunction v_pWriterFunc = v_curSubMesh.getWriterFunction();

		const std::size_t v_quadCount = v_curSubMesh.m_dataIdx.size();
		for (std::size_t a = 0; a < v_quadCount; a++)
		{
			//Put the pointer back to the beginning of the buffer
			g_modelWriterPtr = g_modelWriterBuf;
			*g_modelWriterPtr++ = 'f';

			const std::vector<VertexData>& v_vecVerts = v_curSubMesh.m_dataIdx[a];
			const std::size_t v_vecVertsSz = v_vecVerts.size();

			if (v_vecVertsSz > 30) //The N-Gon is too big to be handled with a fast for loop
			{
				for (std::size_t b = 0; b < 30; b++)
					v_pWriterFunc(v_idxWriterArgs, v_vecVerts[b]);

				for (std::size_t b = 30; b < v_vecVertsSz; b++)
				{
					v_pWriterFunc(v_idxWriterArgs, v_vecVerts[b]);

					//never let the writer reach the end of the buffer
					if ((g_modelWriterBufferEnd - g_modelWriterPtr) < 100)
					{
						DebugOutL("Reaching the end of the model writer buffer, resetting... (Remaining space: ", std::size_t(g_modelWriterBufferEnd - g_modelWriterPtr), ")");

						file.write(g_modelWriterBuf, g_modelWriterPtr - g_modelWriterBuf);
						g_modelWriterPtr = g_modelWriterBuf;
					}
				}
			}
			else
			{
				for (std::size_t b = 0; b < v_vecVertsSz; b++)
					v_pWriterFunc(v_idxWriterArgs, v_vecVerts[b]);
			}

			*g_modelWriterPtr++ = '\n';

			file.write(g_modelWriterBuf, g_modelWriterPtr - g_modelWriterBuf);
		}
	}

	if (!SharedConverterSettings::DeduplicateVertices)
		offset.Vertex += m_vertices.size();
}

////////////////// MODEL STORAGE ///////////////////////

const aiScene* ModelStorage::LoadScene(const std::wstring_view& path)
{
	return Importer.ReadFile(
		String::ToUtf8(path).c_str(),
		aiProcess_FindInvalidData |
		aiProcess_RemoveComponent |
		aiProcess_JoinIdenticalVertices |
		aiProcess_FindDegenerates
	);
}

void ModelStorage::LoadVertices(const aiMesh* mesh, Model* model)
{
	model->m_vertices.insert(
		model->m_vertices.end(),
		reinterpret_cast<const glm::vec3*>(mesh->mVertices),
		reinterpret_cast<const glm::vec3*>(mesh->mVertices) + std::size_t(mesh->mNumVertices)
	);

	if (SharedConverterSettings::ExportNormals && mesh->HasNormals())
	{
		model->m_normals.insert(
			model->m_normals.end(),
			reinterpret_cast<const glm::vec3*>(mesh->mNormals),
			reinterpret_cast<const glm::vec3*>(mesh->mNormals) + std::size_t(mesh->mNumVertices)
		);
	}

	if (SharedConverterSettings::ExportUvs && mesh->HasTextureCoords(0))
	{
		model->m_uvs.reserve(std::size_t(mesh->mNumVertices));
		for (std::uint32_t a = 0; a < mesh->mNumVertices; a++)
			model->m_uvs.emplace_back(*reinterpret_cast<const glm::vec2*>(&mesh->mTextureCoords[0][a]));
	}
}

void ModelStorage::LoadMaterialName(const aiScene* pScene, const aiMesh* pMesh, SubMeshData& subMesh)
{
	if (!pScene->HasMaterials()) return;

	const aiMaterial* cMeshMat = pScene->mMaterials[pMesh->mMaterialIndex];
	aiString cMatName;
	cMeshMat->Get(AI_MATKEY_NAME, cMatName);

	subMesh.m_materialName.assign(cMatName.C_Str(), cMatName.length);
}

void ModelStorage::LoadIndices(const aiMesh* pMesh, Model* pModel, SubMeshData& subMesh)
{
	const std::size_t v_vertOffset = pModel->m_vertices.size();
	const std::size_t v_uvOffset = pModel->m_uvs.size();
	const std::size_t v_normalOffset = pModel->m_normals.size();

	subMesh.m_dataIdx.reserve(pMesh->mNumFaces);
	for (std::uint32_t a = 0; a < pMesh->mNumFaces; a++)
	{
		const aiFace& v_curFace = pMesh->mFaces[a];
		
		std::vector<VertexData>& v_vecVerts = subMesh.m_dataIdx.emplace_back();
		v_vecVerts.reserve(v_curFace.mNumIndices);

		for (std::uint32_t b = 0; b < v_curFace.mNumIndices; b++)
		{
			const std::size_t v_indIdx = (std::size_t)v_curFace.mIndices[b];
			v_vecVerts.emplace_back(
				v_indIdx + v_vertOffset,
				subMesh.m_hasUvs     ? (v_indIdx + v_uvOffset    ) : 0,
				subMesh.m_hasNormals ? (v_indIdx + v_normalOffset) : 0
			);
		}
	}
}

void ModelStorage::LoadSubMeshes(const aiScene* pScene, Model* pModel)
{
	pModel->m_subMeshData.reserve(pScene->mNumMeshes);
	for (std::uint32_t a = 0; a < pScene->mNumMeshes; a++)
	{
		const aiMesh* v_pCurMesh = pScene->mMeshes[a];

		SubMeshData& v_newSubMesh = pModel->m_subMeshData.emplace_back(
			a,
			SharedConverterSettings::ExportNormals && v_pCurMesh->HasNormals(),
			SharedConverterSettings::ExportUvs && v_pCurMesh->HasTextureCoords(0)
		);

		ModelStorage::LoadIndices(v_pCurMesh, pModel, v_newSubMesh);
		ModelStorage::LoadVertices(v_pCurMesh, pModel);
		ModelStorage::LoadMaterialName(pScene, v_pCurMesh, v_newSubMesh);
	}
}

Model* ModelStorage::LoadModel(const std::wstring_view& path)
{
	const auto v_iter = ModelStorage::CachedModels.find(path);
	if (v_iter != ModelStorage::CachedModels.end())
		return v_iter->second;

	const aiScene* v_pScene = ModelStorage::LoadScene(path);
	if (v_pScene && v_pScene->HasMeshes())
	{
		Model* newModel = new Model(path);
		ModelStorage::LoadSubMeshes(v_pScene, newModel);

		ModelStorage::CachedModels.emplace(path, newModel);
		Importer.FreeScene();

		return newModel;
	}

	DebugErrorL("[Model] Couldn't load: ", path);

	Importer.FreeScene();
	return nullptr;
}

void ModelStorage::ClearStorage()
{
	for (auto& model_data : ModelStorage::CachedModels)
		delete model_data.second;

	ModelStorage::CachedModels.clear();
}
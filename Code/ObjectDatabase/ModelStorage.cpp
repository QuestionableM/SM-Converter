#include "ModelStorage.hpp"

#include "Converter\ConvertSettings.hpp"
#include "Converter\Entity\Entity.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"

#pragma unmanaged

static char g_modelWriterBuf[1024];
static char* g_modelWriterPtr;

void SubMeshData::IndexWriter_None(const IndexWriterArguments& v_data, const VertexData& v_vert)
{
	const glm::vec3& v_vertex = (*v_data.translated_vertices)[v_vert.m_Vert];
	const std::size_t& v_vert_idx = v_data.offset->VertexMap.at(v_vertex);

	*g_modelWriterPtr++ = ' ';
	g_modelWriterPtr = String::FromInteger<std::size_t>(v_vert_idx + 1, g_modelWriterPtr);
}

void SubMeshData::IndexWriter_Normals(const IndexWriterArguments& v_data, const VertexData& v_vert)
{
	const glm::vec3& v_vertex = (*v_data.translated_vertices)[v_vert.m_Vert];
	const glm::vec3& v_normal = (*v_data.translated_normals)[v_vert.m_Norm];

	const std::size_t& v_vert_idx = v_data.offset->VertexMap.at(v_vertex);
	const std::size_t& v_norm_idx = v_data.offset->NormalMap.at(v_normal);

	*g_modelWriterPtr++ = ' ';
	g_modelWriterPtr = String::FromInteger<std::size_t>(v_vert_idx + 1, g_modelWriterPtr);
	*g_modelWriterPtr++ = '/';
	*g_modelWriterPtr++ = '/';
	g_modelWriterPtr = String::FromInteger<std::size_t>(v_norm_idx + 1, g_modelWriterPtr);
}

void SubMeshData::IndexWriter_Uvs(const IndexWriterArguments& v_data, const VertexData& v_vert)
{
	const glm::vec3& v_vertex = (*v_data.translated_vertices)[v_vert.m_Vert];
	const glm::vec2& v_uv = v_data.m_model->uvs[v_vert.m_Uv];

	const std::size_t& v_vert_idx = v_data.offset->VertexMap.at(v_vertex);
	const std::size_t& v_uv_idx = v_data.offset->UvMap.at(v_uv);

	*g_modelWriterPtr++ = ' ';
	g_modelWriterPtr = String::FromInteger<std::size_t>(v_vert_idx + 1, g_modelWriterPtr);
	*g_modelWriterPtr++ = '/';
	g_modelWriterPtr = String::FromInteger<std::size_t>(v_uv_idx + 1, g_modelWriterPtr);
}

void SubMeshData::IndexWriter_UvsAndNormals(const IndexWriterArguments& v_data, const VertexData& v_vert)
{
	const glm::vec3& v_vertex = (*v_data.translated_vertices)[v_vert.m_Vert];
	const glm::vec2& v_uv = v_data.m_model->uvs[v_vert.m_Uv];
	const glm::vec3& v_normal = (*v_data.translated_normals)[v_vert.m_Norm];

	const std::size_t& v_vert_idx = v_data.offset->VertexMap.at(v_vertex);
	const std::size_t& v_uv_idx = v_data.offset->UvMap.at(v_uv);
	const std::size_t& v_norm_idx = v_data.offset->NormalMap.at(v_normal);

	*g_modelWriterPtr++ = ' ';
	g_modelWriterPtr = String::FromInteger<std::size_t>(v_vert_idx + 1, g_modelWriterPtr);
	*g_modelWriterPtr++ = '/';
	g_modelWriterPtr = String::FromInteger<std::size_t>(v_uv_idx + 1, g_modelWriterPtr);
	*g_modelWriterPtr++ = '/';
	g_modelWriterPtr = String::FromInteger<std::size_t>(v_norm_idx + 1, g_modelWriterPtr);
}

SubMeshData::IndexWriterFunction SubMeshData::GetWriterFunction() const
{
	const static SubMeshData::IndexWriterFunction v_writer_functions[4] =
	{
		SubMeshData::IndexWriter_None,
		SubMeshData::IndexWriter_Normals,
		SubMeshData::IndexWriter_Uvs,
		SubMeshData::IndexWriter_UvsAndNormals
	};

	const int v_func_idx = static_cast<int>(this->has_normals) | (static_cast<int>(this->has_uvs) << 1);
	return v_writer_functions[v_func_idx];
}

void Model::WriteToFile(const glm::mat4& model_mat, WriterOffsetData& offset, std::ofstream& file, const SMEntity* pEntity)
{
	std::vector<glm::vec3> mTranslatedVertices(this->vertices.size());
	std::vector<glm::vec3> mTranslatedNormals(this->normals.size());

	g_modelWriterBuf[0] = 'v';
	g_modelWriterBuf[1] = ' ';

	for (std::size_t a = 0; a < this->vertices.size(); a++)
	{
		const glm::vec3 pVertPos = model_mat * glm::vec4(this->vertices[a], 1.0f);

		mTranslatedVertices[a] = pVertPos;

		if (offset.VertexMap.find(pVertPos) == offset.VertexMap.end())
		{
			offset.VertexMap.insert(std::make_pair(pVertPos, offset.Vertex));
			offset.Vertex++;

			//This is actually faster than sprintf("%g %g %g")
			g_modelWriterPtr = String::FromFloat(pVertPos.x, g_modelWriterBuf + 2);
			*g_modelWriterPtr++ = ' ';
			g_modelWriterPtr = String::FromFloat(pVertPos.y, g_modelWriterPtr);
			*g_modelWriterPtr++ = ' ';
			g_modelWriterPtr = String::FromFloat(pVertPos.z, g_modelWriterPtr);
			*g_modelWriterPtr++ = '\n';
			*g_modelWriterPtr = '\0';

			file.write(g_modelWriterBuf, g_modelWriterPtr - g_modelWriterBuf);
		}
	}

	if (SharedConverterSettings::ExportUvs)
	{
		g_modelWriterBuf[1] = 't';
		g_modelWriterBuf[2] = ' ';

		for (std::size_t a = 0; a < this->uvs.size(); a++)
		{
			const glm::vec2& uv = this->uvs[a];

			if (offset.UvMap.find(uv) == offset.UvMap.end())
			{
				offset.UvMap.insert(std::make_pair(uv, offset.Uv));
				offset.Uv++;

				g_modelWriterPtr = String::FromFloat(uv.x, g_modelWriterBuf + 3);
				*g_modelWriterPtr++ = ' ';
				g_modelWriterPtr = String::FromFloat(uv.y, g_modelWriterPtr);
				*g_modelWriterPtr++ = '\n';
				*g_modelWriterPtr = '\0';

				file.write(g_modelWriterBuf, g_modelWriterPtr - g_modelWriterBuf);
			}
		}
	}

	if (SharedConverterSettings::ExportNormals)
	{
		g_modelWriterBuf[1] = 'n';
		g_modelWriterBuf[2] = ' ';

		const glm::mat3 v_rot_matrix(model_mat);

		for (std::size_t a = 0; a < this->normals.size(); a++)
		{
			const glm::vec3 pNormal = v_rot_matrix * this->normals[a];

			mTranslatedNormals[a] = pNormal;

			if (offset.NormalMap.find(pNormal) == offset.NormalMap.end())
			{
				offset.NormalMap.insert(std::make_pair(pNormal, offset.Normal));
				offset.Normal++;

				g_modelWriterPtr = String::FromFloat(pNormal.x, g_modelWriterBuf + 3);
				*g_modelWriterPtr++ = ' ';
				g_modelWriterPtr = String::FromFloat(pNormal.y, g_modelWriterPtr);
				*g_modelWriterPtr++ = ' ';
				g_modelWriterPtr = String::FromFloat(pNormal.z, g_modelWriterPtr);
				*g_modelWriterPtr++ = '\n';
				*g_modelWriterPtr = '\0';

				file.write(g_modelWriterBuf, g_modelWriterPtr - g_modelWriterBuf);
			}
		}
	}

	IndexWriterArguments v_idx_writer_arg;
	v_idx_writer_arg.m_model = this;
	v_idx_writer_arg.offset = &offset;
	v_idx_writer_arg.translated_normals = &mTranslatedNormals;
	v_idx_writer_arg.translated_vertices = &mTranslatedVertices;

	for (std::size_t mIdx = 0; mIdx < this->subMeshData.size(); mIdx++)
	{
		const SubMeshData* pSubMesh = this->subMeshData[mIdx];

		if (pEntity != nullptr)
		{
			//Skip writing the sub mesh if entity doesn't allow it
			if (!pEntity->GetCanWrite(pSubMesh->m_MaterialName, mIdx))
				continue;

			if (SharedConverterSettings::ExportMaterials)
			{
				const std::string v_mtl_name = "usemtl " + pEntity->GetMtlName(pSubMesh->m_MaterialName, mIdx) + '\n';
				file.write(v_mtl_name.c_str(), v_mtl_name.size());
			}
		}

		v_idx_writer_arg.m_sub_mesh = pSubMesh;

		SubMeshData::IndexWriterFunction v_writer_func = pSubMesh->GetWriterFunction();

		for (std::size_t a = 0; a < pSubMesh->m_DataIdx.size(); a++)
		{
			//Put the pointer back to the beginning of the buffer
			g_modelWriterPtr = g_modelWriterBuf;
			*g_modelWriterPtr++ = 'f';

			const std::vector<VertexData>& v_vert_vec = pSubMesh->m_DataIdx[a];
			for (std::size_t b = 0; b < v_vert_vec.size(); b++)
				v_writer_func(v_idx_writer_arg, v_vert_vec[b]);

			*g_modelWriterPtr++ = '\n';
			*g_modelWriterPtr = '\0';

			file.write(g_modelWriterBuf, g_modelWriterPtr - g_modelWriterBuf);
		}
	}
}

Model::~Model()
{
	for (SubMeshData*& data_ptr : this->subMeshData)
		delete data_ptr;
}



std::unordered_map<std::wstring, Model*> ModelStorage::CachedModels = {};
Assimp::Importer ModelStorage::Importer = Assimp::Importer();

const aiScene* ModelStorage::LoadScene(const std::wstring& path)
{
	return Importer.ReadFile(
		String::ToUtf8(path).c_str(),
		aiProcess_FindInvalidData |
		aiProcess_RemoveComponent |
		aiProcess_JoinIdenticalVertices |
		aiProcess_FindDegenerates
	);
}

void ModelStorage::LoadVertices(const aiMesh*& mesh, Model*& model)
{
	const bool has_uvs     = (SharedConverterSettings::ExportUvs     && mesh->HasTextureCoords(0));
	const bool has_normals = (SharedConverterSettings::ExportNormals && mesh->HasNormals());

	if (has_uvs) model->uvs.reserve(mesh->mNumVertices);
	if (has_normals) model->normals.reserve(mesh->mNumVertices);

	model->vertices.reserve(mesh->mNumVertices);
	for (unsigned int a = 0; a < mesh->mNumVertices; a++)
	{
		model->vertices.push_back(*reinterpret_cast<glm::vec3*>(&mesh->mVertices[a]));

		if (has_uvs) model->uvs.push_back(*reinterpret_cast<glm::vec2*>(&mesh->mTextureCoords[0][a]));
		if (has_normals) model->normals.push_back(*reinterpret_cast<glm::vec3*>(&mesh->mNormals[a]));
	}
}

void ModelStorage::LoadMaterialName(const aiScene*& scene, const aiMesh*& mesh, SubMeshData*& sub_mesh)
{
	if (!scene->HasMaterials()) return;

	const aiMaterial* cMeshMat = scene->mMaterials[mesh->mMaterialIndex];
	aiString cMatName;
	cMeshMat->Get(AI_MATKEY_NAME, cMatName);

	sub_mesh->m_MaterialName = std::string(cMatName.C_Str(), cMatName.length);
}

void ModelStorage::LoadIndices(const aiMesh*& mesh, Model*& model, SubMeshData*& sub_mesh)
{
	const std::size_t l_VertOffset = model->vertices.size();
	const std::size_t l_UvOffset = model->uvs.size();
	const std::size_t l_NormalOffset = model->normals.size();

	sub_mesh->m_DataIdx.reserve(mesh->mNumFaces);
	for (unsigned int a = 0; a < mesh->mNumFaces; a++)
	{
		const aiFace& cFace = mesh->mFaces[a];
		std::vector<VertexData> d_idx;

		d_idx.reserve(cFace.mNumIndices);
		for (unsigned int b = 0; b < cFace.mNumIndices; b++)
		{
			const std::size_t l_ind_idx = (std::size_t)cFace.mIndices[b];

			VertexData nVert;
			nVert.m_Vert = l_ind_idx + l_VertOffset;
			nVert.m_Uv   = (sub_mesh->has_uvs     ? (l_ind_idx + l_UvOffset)     : 0);
			nVert.m_Norm = (sub_mesh->has_normals ? (l_ind_idx + l_NormalOffset) : 0);

			d_idx.push_back(nVert);
		}

		sub_mesh->m_DataIdx.push_back(d_idx);
	}
}

void ModelStorage::LoadSubMeshes(const aiScene*& scene, Model*& model)
{
	model->subMeshData.reserve(scene->mNumMeshes);
	for (unsigned int a = 0; a < scene->mNumMeshes; a++)
	{
		const aiMesh* cMesh = scene->mMeshes[a];
		SubMeshData* cSubMesh = new SubMeshData(a);

		cSubMesh->has_normals = (SharedConverterSettings::ExportNormals && cMesh->HasNormals());
		cSubMesh->has_uvs     = (SharedConverterSettings::ExportUvs     && cMesh->HasTextureCoords(0));

		ModelStorage::LoadIndices(cMesh, model, cSubMesh);
		ModelStorage::LoadVertices(cMesh, model);
		ModelStorage::LoadMaterialName(scene, cMesh, cSubMesh);

		model->subMeshData.push_back(cSubMesh);
	}
}

Model* ModelStorage::LoadModel(const std::wstring& path)
{
	const ModelMap::const_iterator v_iter = ModelStorage::CachedModels.find(path);
	if (v_iter != ModelStorage::CachedModels.end())
		return v_iter->second;

	const aiScene* ModelScene = ModelStorage::LoadScene(path);
	if (ModelScene && ModelScene->HasMeshes())
	{
		Model* newModel = new Model(path);
		ModelStorage::LoadSubMeshes(ModelScene, newModel);

		CachedModels.insert(std::make_pair(path, newModel));
		Importer.FreeScene();

		return newModel;
	}

	DebugErrorL("[Model] Couldn't load: ", path);

	Importer.FreeScene();
	return nullptr;
}

void ModelStorage::ClearStorage()
{
	for (auto& model_data : CachedModels)
		delete model_data.second;

	CachedModels.clear();
}
#include "ModelStorage.hpp"

#include "Converter\ConvertSettings.hpp"
#include "Converter\Entity\Entity.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"

#include <gtx\matrix_decompose.hpp>

#pragma unmanaged

void Model::WriteToFile(const glm::mat4& model_mat, WriterOffsetData& offset, std::ofstream& file, const SMEntity* pEntity)
{
	std::vector<glm::vec3> mTranslatedVertices = {};
	std::vector<glm::vec3> mTranslatedNormals  = {};

	mTranslatedVertices.resize(this->vertices.size());
	mTranslatedNormals.resize (this->normals.size());

	for (std::size_t a = 0; a < this->vertices.size(); a++)
	{
		const glm::vec3 pVertPos = model_mat * glm::vec4(this->vertices[a], 1.0f);

		mTranslatedVertices[a] = pVertPos;

		if (offset.VertexMap.find(pVertPos) == offset.VertexMap.end())
		{
			offset.VertexMap.insert(std::make_pair(pVertPos, offset.Vertex));
			offset.Vertex++;

			const std::string output_str = "v " + String::FloatVecToString(&pVertPos.x, 3) + "\n";
			file.write(output_str.c_str(), output_str.size());
		}
	}

	if (SharedConverterSettings::ExportUvs)
	{
		for (std::size_t a = 0; a < this->uvs.size(); a++)
		{
			const glm::vec2& uv = this->uvs[a];

			if (offset.UvMap.find(uv) == offset.UvMap.end())
			{
				offset.UvMap.insert(std::make_pair(uv, offset.Uv));
				offset.Uv++;

				const std::string output_str = "vt " + String::FloatVecToString(&uv.x, 2) + "\n";
				file.write(output_str.c_str(), output_str.size());
			}
		}
	}

	if (SharedConverterSettings::ExportNormals)
	{
		const glm::mat4 rot_matrix(model_mat[0], model_mat[1], model_mat[2], glm::highp_vec4(0.0f, 0.0f, 0.0f, 1.0f));

		for (std::size_t a = 0; a < this->normals.size(); a++)
		{
			const glm::vec3 pNormal = rot_matrix * glm::vec4(this->normals[a], 1.0f);

			mTranslatedNormals[a] = pNormal;

			if (offset.NormalMap.find(pNormal) == offset.NormalMap.end())
			{
				offset.NormalMap.insert(std::make_pair(pNormal, offset.Normal));
				offset.Normal++;

				const std::string output_str = "vn " + String::FloatVecToString(&pNormal.x, 3) + "\n";
				file.write(output_str.c_str(), output_str.size());
			}
		}
	}

	for (std::size_t mIdx = 0; mIdx < this->subMeshData.size(); mIdx++)
	{
		const SubMeshData* pSubMesh = this->subMeshData[mIdx];

		if (pEntity != nullptr && SharedConverterSettings::ExportMaterials)
		{
			const std::string mtl_name = "usemtl " + pEntity->GetMtlName(pSubMesh->m_MaterialName, mIdx) + "\n";
			file.write(mtl_name.c_str(), mtl_name.size());
		}

		for (std::size_t a = 0; a < pSubMesh->m_DataIdx.size(); a++)
		{
			std::string _f_str = "f";

			const std::vector<VertexData>& vert_vec = pSubMesh->m_DataIdx[a];
			for (std::size_t b = 0; b < vert_vec.size(); b++)
			{
				const VertexData& d_idx = vert_vec[b];

				const glm::vec3& l_Vertex = mTranslatedVertices[d_idx.m_Vert];
				const WriterOffsetData::Vec3Iterator v_vert_iter = offset.VertexMap.find(l_Vertex);
				if (v_vert_iter != offset.VertexMap.end())
					_f_str.append(" " + std::to_string(v_vert_iter->second + 1));

				if (!pSubMesh->has_uvs && !pSubMesh->has_normals) continue;

				_f_str.append("/");

				if (pSubMesh->has_uvs)
				{
					const WriterOffsetData::Vec2Iterator v_uv_iter = offset.UvMap.find(this->uvs[d_idx.m_Uv]);
					if (v_uv_iter != offset.UvMap.end())
						_f_str.append(std::to_string(v_uv_iter->second + 1));
				}

				if (pSubMesh->has_normals)
				{
					const glm::vec3& l_Normal = mTranslatedNormals[d_idx.m_Norm];
					const WriterOffsetData::Vec3Iterator v_norm_iter = offset.NormalMap.find(l_Normal);
					if (v_norm_iter != offset.NormalMap.end())
						_f_str.append("/" + std::to_string(v_norm_iter->second + 1));
				}
			}

			_f_str.append("\n");
			file.write(_f_str.c_str(), _f_str.size());
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

	const std::string cStrMatName = std::string(cMatName.C_Str(), cMatName.length);
	sub_mesh->m_MaterialName = String::ToWide(cStrMatName);
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
#pragma once

#include "Converter\WriterOffset.hpp"

#include <unordered_map>
#include <fstream>
#include <string>
#include <vector>

#include <assimp\postprocess.h>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>

#include <glm.hpp>

struct VertexData
{
	std::size_t m_Vert;
	std::size_t m_Uv;
	std::size_t m_Norm;
};

struct SubMeshData
{
	std::wstring m_MaterialName;
	unsigned int m_SubMeshIdx;

	std::vector<std::vector<VertexData>> m_DataIdx;

	bool has_normals;
	bool has_uvs;

	inline bool IsEmpty() { return m_DataIdx.empty(); }

	inline SubMeshData(const int& sub_mesh_idx) { this->m_SubMeshIdx = sub_mesh_idx; }
	~SubMeshData() = default;
};

struct Model
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<SubMeshData*> subMeshData;

	std::wstring meshPath;

	void WriteToFile(const glm::mat4& model_mat, WriterOffsetData& offset, std::ofstream& file, const class SMEntity* pEntity);

	inline bool IsEmpty() const
	{
		return (this->subMeshData.size() <= 0 || (this->vertices.size() <= 0 && this->uvs.size() <= 0 && this->normals.size() <= 0));
	}

	inline Model(const std::wstring& mesh_path) { this->meshPath = mesh_path; }
	Model() = default;
	Model(const Model&) = delete;
	Model(Model&) = delete;
	~Model();
};

class ModelStorage
{
	using ModelMap = std::unordered_map<std::wstring, Model*>;

	static ModelMap CachedModels;
	static Assimp::Importer Importer;

	static const aiScene* LoadScene(const std::wstring& path);
	static void LoadVertices(const aiMesh*& mesh, Model*& model);
	static void LoadMaterialName(const aiScene*& scene, const aiMesh*& mesh, SubMeshData*& sub_mesh);
	static void LoadIndices(const aiMesh*& mesh, Model*& model, SubMeshData*& sub_mesh);
	static void LoadSubMeshes(const aiScene*& scene, Model*& model);
public:
	static Model* LoadModel(const std::wstring& path);

	static void ClearStorage();
};
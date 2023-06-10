#pragma once

#include "Converter\WriterOffset.hpp"

#include "UStd\UnmanagedUnorderedMap.hpp"
#include "UStd\UnmanagedFstream.hpp"
#include "UStd\UnmanagedString.hpp"
#include "UStd\UnmanagedVector.hpp"

#include "Utils\GlmUnmanaged.hpp"

#pragma unmanaged

#include <assimp\postprocess.h>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>

struct VertexData
{
	std::size_t m_Vert;
	std::size_t m_Uv;
	std::size_t m_Norm;
};

struct IndexWriterArguments
{
	const struct Model* m_model;
	const struct SubMeshData* m_sub_mesh;
	WriterOffsetData* offset;

	std::vector<glm::vec3>* translated_vertices;
	std::vector<glm::vec3>* translated_normals;
};

struct SubMeshData
{
	using IndexWriterFunction = void(*)(const IndexWriterArguments&, const VertexData&);

	static void IndexWriter_None(const IndexWriterArguments& v_data, const VertexData& v_vert);
	static void IndexWriter_Normals(const IndexWriterArguments& v_data, const VertexData& v_vert);
	static void IndexWriter_Uvs(const IndexWriterArguments& v_data, const VertexData& v_vert);
	static void IndexWriter_UvsAndNormals(const IndexWriterArguments& v_data, const VertexData& v_vert);

	IndexWriterFunction GetWriterFunction() const;

	inline bool IsEmpty() const noexcept { return m_DataIdx.empty(); }

	inline SubMeshData(int sub_mesh_idx) noexcept { this->m_SubMeshIdx = sub_mesh_idx; }
	~SubMeshData() = default;

	std::string m_MaterialName;
	unsigned int m_SubMeshIdx;

	std::vector<std::vector<VertexData>> m_DataIdx;

	bool has_normals;
	bool has_uvs;
};

struct Model
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<SubMeshData*> subMeshData;

	std::wstring meshPath;
	bool has_cached_uvs = false;

	void WriteToFile(const glm::mat4& model_mat, WriterOffsetData& offset, std::ofstream& file, const class SMEntity* pEntity);

	inline bool IsEmpty() const noexcept
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

	inline static ModelMap CachedModels = {};
	inline static Assimp::Importer Importer = Assimp::Importer();

	static const aiScene* LoadScene(const std::wstring& path);
	static void LoadVertices(const aiMesh*& mesh, Model*& model);
	static void LoadMaterialName(const aiScene*& scene, const aiMesh*& mesh, SubMeshData*& sub_mesh);
	static void LoadIndices(const aiMesh*& mesh, Model*& model, SubMeshData*& sub_mesh);
	static void LoadSubMeshes(const aiScene*& scene, Model*& model);
public:
	static Model* LoadModel(const std::wstring& path);

	static void ClearStorage();
};

#pragma managed
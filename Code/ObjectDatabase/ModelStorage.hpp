#pragma once

#include "Converter/WriterOffset.hpp"

#include "UStd/UnmanagedUnorderedMap.hpp"
#include "UStd/UnmanagedFstream.hpp"
#include "UStd/UnmanagedString.hpp"
#include "UStd/UnmanagedVector.hpp"

#include "Utils/GlmUnmanaged.hpp"
#include "Utils/clr_include.hpp"
#include "Utils/Hashing.hpp"

SM_UNMANAGED_CODE

#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

struct VertexData
{
	inline VertexData(std::size_t vertIdx, std::size_t uvIdx, std::size_t normIdx) noexcept :
		m_vert(vertIdx),
		m_uv(uvIdx),
		m_norm(normIdx)
	{}

	std::size_t m_vert;
	std::size_t m_uv;
	std::size_t m_norm;
};

struct IndexWriterArguments
{
	IndexWriterArguments(
		const struct Model* pModel,
		WriterOffsetData& offsetData,
		const std::vector<glm::vec3>& translatedVertices,
		const std::vector<glm::vec3>& translatedNormals
	) :
		m_model(pModel),
		m_subMesh(nullptr),
		m_offset(offsetData),
		m_translatedVertices(translatedVertices),
		m_translatedNormals(translatedNormals)
	{}

	const struct Model* m_model;
	const struct SubMeshData* m_subMesh;
	WriterOffsetData& m_offset;

	const std::vector<glm::vec3>& m_translatedVertices;
	const std::vector<glm::vec3>& m_translatedNormals;
};

struct SubMeshData
{
	using IndexWriterFunction = void(*)(const IndexWriterArguments&, const VertexData&);

	SubMeshData(
		const std::uint32_t idx,
		const bool hasNormals,
		const bool hasUvs
	);

	SubMeshData(
		const std::uint32_t idx,
		const bool hasNormals,
		const bool hasUvs,
		const std::initializer_list<std::initializer_list<VertexData>>& dataIdx
	);

	SubMeshData(SubMeshData&& other) noexcept;
	~SubMeshData() = default;

	static void IndexWriter_None(const IndexWriterArguments& data, const VertexData& vert);
	static void IndexWriter_NoneUnoptimized(const IndexWriterArguments& data, const VertexData& vert);

	static void IndexWriter_Normals(const IndexWriterArguments& data, const VertexData& vert);
	static void IndexWriter_NormalsUnoptimized(const IndexWriterArguments& data, const VertexData& vert);

	static void IndexWriter_Uvs(const IndexWriterArguments& data, const VertexData& vert);
	static void IndexWriter_UvsUnoptimized(const IndexWriterArguments& data, const VertexData& vert);

	static void IndexWriter_UvsAndNormals(const IndexWriterArguments& data, const VertexData& vert);
	static void IndexWriter_UvsAndNormalsUnoptimized(const IndexWriterArguments& data, const VertexData& vert);

	IndexWriterFunction getWriterFunction() const noexcept;
	inline bool isEmpty() const noexcept { return m_dataIdx.empty(); }

	std::string m_materialName;

	std::vector<std::vector<VertexData>> m_dataIdx;

	std::uint32_t m_subMeshIdx;
	bool m_hasNormals;
	bool m_hasUvs;
};

struct Model
{
	Model(const std::wstring_view& mesh_path);
	Model();

	Model(const Model&) = delete;
	Model(Model&) = delete;

	void WriteToFile(const glm::mat4& model_mat, WriterOffsetData& offset, std::ofstream& file, const class SMEntity* pEntity);
	void WriteToFileGltf(GltfWriterContext& context, const glm::mat4& modelMatrix, const class SMEntity* pEntity);

	inline bool IsEmpty() const noexcept
	{
		return m_subMeshData.empty() || (m_vertices.empty() && m_uvs.empty() && m_normals.empty());
	}

	std::wstring m_path;

	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::vec2> m_uvs;
	std::vector<SubMeshData> m_subMeshData;

	bool m_bUvsCached;
};

class ModelStorage
{
	using ModelMap = std::unordered_map<std::wstring, Model*, Hashing::StringHasher, std::equal_to<>>;

	inline static ModelMap CachedModels = {};
	inline static Assimp::Importer Importer = Assimp::Importer();

	static const aiScene* LoadScene(const std::wstring_view& path);
	static void LoadVertices(const aiMesh* mesh, Model* model);
	static void LoadMaterialName(const aiScene* pScene, const aiMesh* pMesh, SubMeshData& subMesh);
	static void LoadIndices(const aiMesh* pMesh, Model* pModel, SubMeshData& subMesh);
	static void LoadSubMeshes(const aiScene* pScene, Model* pModel);
public:
	static Model* LoadModel(const std::wstring_view& path);

	static void ClearStorage();
};

SM_MANAGED_CODE
#pragma once

#include "Utils/GlmUnmanaged.hpp"
#include "Utils/clr_include.hpp"
#include "Utils/Hashing.hpp"

SM_UNMANAGED_CODE

#include <cstdint>
#include <fstream>
#include <vector>

#include <hash_table8.hpp>

enum class GltfComponentType : std::uint16_t
{
	SIGNED_BYTE    = 5120,
	UNSIGNED_BYTE  = 5121,
	SIGNED_SHORT   = 5122,
	UNSIGNED_SHORT = 5123,
	UNSIGNED_INT   = 5125,
	FLOAT          = 5126
};

enum class GltfAccessorType : std::uint8_t
{
	NONE   = 0,
	SCALAR = 1,
	VEC2   = 2,
	VEC3   = 3,
	VEC4   = 4,
	MAT2   = 5,
	MAT3   = 6,
	MAT4   = 7
};

enum class GltfBufferViewTarget : std::uint32_t
{
	ARRAY_BUFFER         = 34962,
	ELEMENT_ARRAY_BUFFER = 34963
};

std::string_view GltfAccessorTypeToString(const GltfAccessorType type) noexcept;

struct GltfBufferView
{
	GltfBufferView(const std::size_t byteLength, const std::size_t byteOffset) noexcept;

	std::size_t m_byteLength;
	std::size_t m_byteOffset;
	GltfBufferViewTarget m_target;
};

struct GltfBufferAccessor
{
	GltfBufferAccessor(
		const std::size_t viewIdx,
		const std::size_t viewOffset,
		const GltfComponentType componentType,
		const std::size_t itemCount,
		const GltfAccessorType type
	) noexcept;

	std::size_t m_bufferViewIdx;
	std::size_t m_bufferOffset;
	GltfComponentType m_componentType;
	std::size_t m_itemCount;
	GltfAccessorType m_type;
};

struct GltfMeshPrimitive
{
	GltfMeshPrimitive() noexcept;

	std::size_t m_vertexAccessorIdx;
	std::size_t m_uvAccessorIdx;
	std::size_t m_normalAccessorIdx;
	std::size_t m_indexAccessorIdx;
	std::size_t m_materialIdx;
};

struct GltfMesh
{
	GltfMesh();

	GltfMeshPrimitive& createNewPrimitive();

	std::wstring m_name;
	std::vector<GltfMeshPrimitive> m_vecPrimitives;
};

struct GltfObject
{
	std::wstring m_name;
	std::size_t m_meshIdx;
	glm::mat4 m_matrix;
};

struct GltfWriterContext
{
	GltfWriterContext(const std::wstring& outputFile);

	GltfBufferView& createNewView(const std::size_t offset, const std::size_t length);
	GltfBufferAccessor& createNewAccessor(
		const std::size_t viewIdx,
		const std::size_t viewOffset,
		const GltfComponentType componentType,
		const std::size_t itemCount,
		const GltfAccessorType type);

	GltfMesh& getOrCreateNewGroup(const std::wstring_view& name);

	void writeToFile(const void* buffer, const std::size_t bufferSize);
	void writeGltfToFile(const std::wstring& filePath);

	std::ofstream m_outFile;
	std::size_t m_bytesWritten;
	std::size_t m_selectedMeshIdx = std::size_t(-1);

	std::vector<GltfBufferView> m_vecBufferViews;
	std::vector<GltfBufferAccessor> m_vecBufferAccessors;
	std::vector<GltfMesh> m_vecMeshes;
	std::vector<GltfObject> m_vecObjects;

	emhash8::HashMap<std::wstring, std::size_t, Hashing::WstringHasher, std::equal_to<>> m_mapPathToMeshIndex;
	emhash8::HashMap<std::wstring, std::size_t, Hashing::WstringHasher, std::equal_to<>> m_mapGroupNameToIndex;
};

struct WriterOffsetData
{
	emhash8::HashMap<glm::vec3, std::size_t> VertexMap = {};
	emhash8::HashMap<glm::vec3, std::size_t> NormalMap = {};
	emhash8::HashMap<glm::vec2, std::size_t> UvMap = {};

	std::size_t Vertex = 1;
	std::size_t Normal = 1;
	std::size_t Uv = 1;

	WriterOffsetData() = default;
	WriterOffsetData(const WriterOffsetData&) = delete;
	WriterOffsetData(WriterOffsetData&&) = delete;
	~WriterOffsetData() = default;
};

SM_MANAGED_CODE
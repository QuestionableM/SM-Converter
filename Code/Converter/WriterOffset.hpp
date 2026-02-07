#pragma once

#include "Utils/GlmUnmanaged.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

#include <cstdint>
#include <fstream>
#include <vector>

#include <hash_table8.hpp>

enum class GltfComponentType : std::uint16_t
{
	SIGNED_BYTE = 5120,
	UNSIGNED_BYTE = 5121,
	SIGNED_SHORT = 5122,
	UNSIGNED_SHORT = 5123,
	UNSIGNED_INT = 5125,
	FLOAT = 5126
};

enum class GltfAccessorType : std::uint8_t
{
	NONE = 0,
	SCALAR = 1,
	VEC2 = 2,
	VEC3 = 3,
	VEC4 = 4,
	MAT2 = 5,
	MAT3 = 6,
	MAT4 = 7
};

static std::string_view GltfAccessorTypeToString(const GltfAccessorType type) noexcept
{
	switch (type)
	{
	case GltfAccessorType::SCALAR:
		return "SCALAR";
	case GltfAccessorType::VEC2:
		return "VEC2";
	case GltfAccessorType::VEC3:
		return "VEC3";
	case GltfAccessorType::VEC4:
		return "VEC4";
	case GltfAccessorType::MAT2:
		return "MAT2";
	case GltfAccessorType::MAT3:
		return "MAT3";
	case GltfAccessorType::MAT4:
		return "MAT4";
	default:
		return "UNKNOWN";
	}
}

enum class GltfBufferViewTarget : std::uint32_t
{
	ARRAY_BUFFER = 34962,
	ELEMENT_ARRAY_BUFFER = 34963
};

struct GltfBufferView
{
	inline GltfBufferView(
		const std::size_t byteLength,
		const std::size_t byteOffset
	) noexcept
		: m_byteLength(byteLength)
		, m_byteOffset(byteOffset)
		, m_target(GltfBufferViewTarget::ARRAY_BUFFER)
	{}

	std::size_t m_byteLength;
	std::size_t m_byteOffset;
	GltfBufferViewTarget m_target;
};

struct GltfBufferAccessor
{
	inline GltfBufferAccessor(
		const std::size_t viewIdx,
		const std::size_t viewOffset,
		const GltfComponentType componentType,
		const std::size_t itemCount,
		const GltfAccessorType type
	) noexcept
		: m_bufferViewIdx(viewIdx)
		, m_bufferOffset(viewOffset)
		, m_componentType(componentType)
		, m_itemCount(itemCount)
		, m_type(type)
	{}

	std::size_t m_bufferViewIdx;
	std::size_t m_bufferOffset;
	GltfComponentType m_componentType;
	std::size_t m_itemCount;
	GltfAccessorType m_type;
};

struct GltfMesh
{
	inline GltfMesh()
		: m_name()
		, m_materialName()
		, m_vertexAccessorIdx(std::size_t(-1))
		, m_uvAccessorIdx(std::size_t(-1))
		, m_normalAccessorIdx(std::size_t(-1))
		, m_indexAccessorIdx(std::size_t(-1))
	{}

	std::wstring m_name;
	std::wstring m_materialName;
	std::size_t m_vertexAccessorIdx;
	std::size_t m_uvAccessorIdx;
	std::size_t m_normalAccessorIdx;
	std::size_t m_indexAccessorIdx;
};

struct GltfObject
{
	std::wstring m_name;
	std::size_t m_meshIdx;
	glm::mat4 m_matrix;
};

struct GltfWriterContext
{
	inline GltfWriterContext(const std::wstring& outputFile)
		: m_outFile(outputFile.c_str(), std::ios::binary)
		, m_bytesWritten(0)
		, m_vecBufferViews()
		, m_vecBufferAccessors()
		, m_vecMeshes()
		, m_vecObjects()
		, m_mapPathToMeshIndex()
	{}

	GltfBufferView& createNewView(const std::size_t offset, const std::size_t length)
	{
		return m_vecBufferViews.emplace_back(length, offset);
	}

	GltfBufferAccessor& createNewAccessor(
		const std::size_t viewIdx,
		const std::size_t viewOffset,
		const GltfComponentType componentType,
		const std::size_t itemCount,
		const GltfAccessorType type)
	{
		return m_vecBufferAccessors.emplace_back(viewIdx, viewOffset, componentType, itemCount, type);
	}

	void writeToFile(const void* buffer, const std::size_t bufferSize)
	{
		m_outFile.write(reinterpret_cast<const char*>(buffer), bufferSize);
		m_bytesWritten += bufferSize;
	}


	std::ofstream m_outFile;
	std::size_t m_bytesWritten;

	std::vector<GltfBufferView> m_vecBufferViews;
	std::vector<GltfBufferAccessor> m_vecBufferAccessors;
	std::vector<GltfMesh> m_vecMeshes;
	std::vector<GltfObject> m_vecObjects;

	emhash8::HashMap<std::wstring, std::size_t> m_mapPathToMeshIndex;
};

struct WriterOffsetData
{
	emhash8::HashMap<glm::vec3, std::size_t> VertexMap = {};
	emhash8::HashMap<glm::vec3, std::size_t> NormalMap = {};
	emhash8::HashMap<glm::vec2, std::size_t> UvMap = {};
	//std::unordered_map<glm::vec3, std::size_t> VertexMap = {};
	//std::unordered_map<glm::vec3, std::size_t> NormalMap = {};
	//std::unordered_map<glm::vec2, std::size_t> UvMap     = {};

	std::size_t Vertex = 1;
	std::size_t Normal = 1;
	std::size_t Uv = 1;

	WriterOffsetData() = default;
	WriterOffsetData(const WriterOffsetData&) = delete;
	WriterOffsetData(WriterOffsetData&&) = delete;
	~WriterOffsetData() = default;
};

SM_MANAGED_CODE
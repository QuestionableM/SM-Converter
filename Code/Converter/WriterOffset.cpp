#include "WriterOffset.hpp"
#include "Utils/Json.hpp"

SM_UNMANAGED_CODE

std::string_view GltfAccessorTypeToString(const GltfAccessorType type) noexcept
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

////////////////// GLTF BUFFER VIEW //////////////////

GltfBufferView::GltfBufferView(
	const std::size_t byteLength,
	const std::size_t byteOffset
) noexcept
	: m_byteLength(byteLength)
	, m_byteOffset(byteOffset)
	, m_target(GltfBufferViewTarget::ARRAY_BUFFER)
{}

//////////////// GLTF BUFFER ACCESSOR /////////////////

GltfBufferAccessor::GltfBufferAccessor(
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

/////////////// GLTF MESH PRIMITIVE ////////////////

GltfMeshPrimitive::GltfMeshPrimitive() noexcept
	: m_vertexAccessorIdx(std::size_t(-1))
	, m_uvAccessorIdx(std::size_t(-1))
	, m_normalAccessorIdx(std::size_t(-1))
	, m_indexAccessorIdx(std::size_t(-1))
	, m_materialIdx(std::size_t(-1))
{}

////////////// GLTF MESH ////////////////

GltfMesh::GltfMesh()
	: m_name()
	, m_vecPrimitives()
{}

GltfMeshPrimitive& GltfMesh::createNewPrimitive()
{
	return m_vecPrimitives.emplace_back();
}

//////////// GLTF WRITER CONTEXT ////////////

GltfWriterContext::GltfWriterContext(
	const std::wstring& outputFile
)
	: m_outFile(outputFile.c_str(), std::ios::binary)
	, m_bytesWritten(0)
	, m_vecBufferViews()
	, m_vecBufferAccessors()
	, m_vecMeshes()
	, m_vecObjects()
	, m_mapPathToMeshIndex()
	, m_mapGroupNameToIndex()
{}

GltfBufferView& GltfWriterContext::createNewView(
	const std::size_t offset,
	const std::size_t length)
{
	return m_vecBufferViews.emplace_back(length, offset);
}

GltfBufferAccessor& GltfWriterContext::createNewAccessor(
	const std::size_t viewIdx,
	const std::size_t viewOffset,
	const GltfComponentType componentType,
	const std::size_t itemCount,
	const GltfAccessorType type)
{
	return m_vecBufferAccessors.emplace_back(viewIdx, viewOffset, componentType, itemCount, type);
}

GltfMesh& GltfWriterContext::getOrCreateNewGroup(
	const std::wstring_view& name)
{
	auto v_iter = m_mapGroupNameToIndex.find(name);
	if (v_iter != m_mapGroupNameToIndex.end())
	{
		m_selectedMeshIdx = v_iter->second;
		return m_vecMeshes[v_iter->second];
	}

	m_mapGroupNameToIndex.emplace(name, m_vecMeshes.size());
	m_selectedMeshIdx = m_vecMeshes.size();

	auto v_newObject = m_vecObjects.emplace_back();
	v_newObject.m_meshIdx = m_selectedMeshIdx;
	v_newObject.m_name = name;

	return m_vecMeshes.emplace_back();
}

void GltfWriterContext::writeToFile(const void* buffer, const std::size_t bufferSize)
{
	m_outFile.write(reinterpret_cast<const char*>(buffer), bufferSize);
	m_bytesWritten += bufferSize;
}

void GltfWriterContext::writeGltfToFile(const std::wstring& filePath)
{
	nlohmann::json v_root(nlohmann::json::value_t::object);
	v_root["scene"] = 0;

	{
		nlohmann::json v_asset(nlohmann::json::value_t::object);
		v_asset["generator"] = "Khronos glTF SM-Converter";
		v_asset["version"] = "2.0";

		v_root["asset"] = std::move(v_asset);
	}

	{
		nlohmann::json v_scenes(nlohmann::json::value_t::array);

		{
			nlohmann::json v_sceneObject(nlohmann::json::value_t::object);
			v_sceneObject["name"] = "Scene";

			{
				nlohmann::json v_nodeArray(nlohmann::json::value_t::array);

				for (std::size_t a = 0; a < m_vecObjects.size(); a++)
					v_nodeArray.emplace_back(a);

				v_sceneObject["nodes"] = std::move(v_nodeArray);
			}

			v_scenes.emplace_back(std::move(v_sceneObject));
		}

		v_root["scenes"] = std::move(v_scenes);
	}

	{
		nlohmann::json v_nodeArray(nlohmann::json::value_t::array);

		for (const auto& v_curObject : m_vecObjects)
		{
			nlohmann::json v_curObjectJson(nlohmann::json::value_t::object);
			v_curObjectJson["mesh"] = v_curObject.m_meshIdx;

			v_nodeArray.emplace_back(std::move(v_curObjectJson));
		}

		v_root["nodes"] = std::move(v_nodeArray);
	}

	{
		nlohmann::json v_accessorArray(nlohmann::json::value_t::array);

		for (const auto& v_curAccessor : m_vecBufferAccessors)
		{
			nlohmann::json v_curAccessorJson(nlohmann::json::value_t::object);
			v_curAccessorJson["componentType"] = static_cast<std::uint32_t>(v_curAccessor.m_componentType);
			v_curAccessorJson["byteOffset"] = v_curAccessor.m_bufferOffset;
			v_curAccessorJson["bufferView"] = v_curAccessor.m_bufferViewIdx;
			v_curAccessorJson["count"] = v_curAccessor.m_itemCount;
			v_curAccessorJson["type"] = GltfAccessorTypeToString(v_curAccessor.m_type);

			v_accessorArray.emplace_back(std::move(v_curAccessorJson));
		}

		v_root["accessors"] = std::move(v_accessorArray);
	}

	{
		nlohmann::json v_meshArray(nlohmann::json::value_t::array);

		for (const auto& v_curMesh : m_vecMeshes)
		{
			nlohmann::json v_curMeshJson(nlohmann::json::value_t::object);
			v_curMeshJson["name"] = "Test";

			{
				nlohmann::json v_primitivesArray(nlohmann::json::value_t::array);

				for (const auto& v_curPrimitive : v_curMesh.m_vecPrimitives)
				{
					nlohmann::json v_primitiveObject(nlohmann::json::value_t::object);
					v_primitiveObject["indices"] = v_curPrimitive.m_indexAccessorIdx;

					if (v_curPrimitive.m_materialIdx != std::size_t(-1))
						v_primitiveObject["material"] = v_curPrimitive.m_materialIdx;

					{
						nlohmann::json v_attributesObject(nlohmann::json::value_t::object);

						if (v_curPrimitive.m_vertexAccessorIdx != std::size_t(-1))
							v_attributesObject["POSITION"] = v_curPrimitive.m_vertexAccessorIdx;

						if (v_curPrimitive.m_uvAccessorIdx != std::size_t(-1))
							v_attributesObject["TEXTURE_0"] = v_curPrimitive.m_uvAccessorIdx;

						if (v_curPrimitive.m_normalAccessorIdx != std::size_t(-1))
							v_attributesObject["NORMAL"] = v_curPrimitive.m_normalAccessorIdx;

						v_primitiveObject["attributes"] = std::move(v_attributesObject);
					}

					v_primitivesArray.emplace_back(std::move(v_primitiveObject));
				}

				v_curMeshJson["primitives"] = std::move(v_primitivesArray);
			}

			v_meshArray.emplace_back(std::move(v_curMeshJson));
		}

		v_root["meshes"] = std::move(v_meshArray);
	}

	{
		nlohmann::json v_bufferViewArray(nlohmann::json::value_t::array);

		for (const auto& v_curView : m_vecBufferViews)
		{
			nlohmann::json v_curViewObject(nlohmann::json::value_t::object);
			v_curViewObject["buffer"] = 0;
			v_curViewObject["byteLength"] = v_curView.m_byteLength;
			v_curViewObject["byteOffset"] = v_curView.m_byteOffset;
			v_curViewObject["target"] = v_curView.m_target;

			v_bufferViewArray.emplace_back(std::move(v_curViewObject));
		}

		v_root["bufferViews"] = std::move(v_bufferViewArray);
	}

	{
		nlohmann::json v_bufferArray(nlohmann::json::value_t::array);

		{
			nlohmann::json v_bufferObject(nlohmann::json::value_t::object);
			v_bufferObject["byteLength"] = m_bytesWritten;
			v_bufferObject["uri"] = "mesh_data.bin";

			v_bufferArray.emplace_back(std::move(v_bufferObject));
		}

		v_root["buffers"] = std::move(v_bufferArray);
	}

	std::ofstream v_gltfOutput(filePath);
	if (v_gltfOutput.is_open())
		v_gltfOutput << v_root;
}
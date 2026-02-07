#include "BlueprintConverter.hpp"

#include "ObjectDatabase/Mods/CustomGameSwitch.hpp"
#include "ObjectDatabase/DatabaseConfig.hpp"
#include "ObjectDatabase/ProgCounter.hpp"

#include "Converter/ConvertSettings.hpp"
#include "Converter/MtlFileWriter.hpp"

#include "Utils/Console.hpp"
#include "Utils/File.hpp"

SM_UNMANAGED_CODE

void BlueprintConv::WriteToFileInternal(SMBlueprint* pBlueprint, const std::wstring& blueprintName, ConvertError& error)
{
	if (error) return;

	const std::wstring v_bpOutDir(DatabaseConfig::BlueprintOutputFolder);
	if (!File::CreateDirectorySafe(v_bpOutDir))
	{
		error.setError(1, "Couldn't create the main output directory");
		return;
	}

	const std::wstring v_bpDirPath = v_bpOutDir + L"/" + blueprintName;
	if (!File::CreateDirectorySafe(v_bpDirPath))
	{
		error.setError(1, "Couldn't create the blueprint output directory");
		return;
	}

	const std::wstring v_bpOutputPath = v_bpDirPath + L"/" + blueprintName;

#if 0 // OBJ implementation
	{
		//Write object file
		std::ofstream v_obj_writer(v_bpOutputPath + L".obj");
		if (!v_obj_writer.is_open())
		{
			error.setError(1, "Couldn't create an object file");
			return;
		}

		glm::vec3 v_center_point(0.0f);

		const std::size_t v_bp_object_count = pBlueprint->GetAmountOfObjects();
		if (v_bp_object_count > 0) //prevent division by 0 exception
		{
			pBlueprint->CalculateCenterPoint(v_center_point);
			v_center_point /= static_cast<float>(v_bp_object_count);
		}

		v_center_point *= pBlueprint->m_size;

		ProgCounter::SetState(ProgState::WritingObjects, v_bp_object_count);

		WriterOffsetData v_offset_data;
		pBlueprint->WriteObjectToFile(v_obj_writer, v_offset_data, glm::translate(-v_center_point));
		v_obj_writer.close();
	}

	{
		ProgCounter::SetState(ProgState::WritingMtlFile, 0);

		SMEntity::EntityTextureMap v_textureMap;

		for (const SMEntity* v_entity : pBlueprint->m_objects)
		{
			v_entity->FillTextureMap(v_textureMap);
			ProgCounter::ProgressMax = v_textureMap.size();
		}

		MtlFileWriter::Write(v_bpOutputPath + L".mtl", v_textureMap);
	}
#else
	ProgCounter::SetState(ProgState::WritingObjects, pBlueprint->GetAmountOfObjects());

	const std::wstring v_bpBinaryPath = v_bpDirPath + L"/mesh_data.bin";

	GltfWriterContext v_writerContext(v_bpBinaryPath);
	if (!v_writerContext.m_outFile.is_open())
	{
		error.setError(1, "Couldn't create an object file");
		return;
	}

	pBlueprint->WriteObjectToFileGltf(v_writerContext, glm::mat4(1.0f));

	// Write the gltf file itself
	nlohmann::json v_root = nlohmann::json::object();
	v_root["scene"] = 0;

	{
		nlohmann::json v_asset = nlohmann::json::object();
		v_asset["generator"] = "Khronos glTF SM-Converter";
		v_asset["version"] = "2.0";

		v_root["asset"] = std::move(v_asset);
	}

	{
		nlohmann::json v_scenes = nlohmann::json::array();

		{
			nlohmann::json v_sceneObject = nlohmann::json::object();
			v_sceneObject["name"] = "Scene";
		
			{
				nlohmann::json v_nodesArray = nlohmann::json::array();

				for (std::size_t a = 0; a < v_writerContext.m_vecObjects.size(); a++)
					v_nodesArray.push_back(a);

				v_sceneObject["nodes"] = std::move(v_nodesArray);
			}
			
			v_scenes.push_back(std::move(v_sceneObject));
		}
		
		v_root["scenes"] = std::move(v_scenes);
	}

	{
		nlohmann::json v_nodesArray = nlohmann::json::array();

		for (const auto& v_curObject : v_writerContext.m_vecObjects)
		{
			nlohmann::json v_curObjectJson = nlohmann::json::object();
			v_curObjectJson["mesh"] = v_curObject.m_meshIdx;
			// v_curObjectJson["name"] = v_curObject.m_name;

			nlohmann::json v_matrixArray = nlohmann::json::array();
			for (std::size_t a = 0; a < 4; a++)
			{
				v_matrixArray.push_back(v_curObject.m_matrix[a].x);
				v_matrixArray.push_back(v_curObject.m_matrix[a].y);
				v_matrixArray.push_back(v_curObject.m_matrix[a].z);
				v_matrixArray.push_back(v_curObject.m_matrix[a].w);
			}

			v_curObjectJson["matrix"] = std::move(v_matrixArray);

			v_nodesArray.push_back(std::move(v_curObjectJson));
		}

		v_root["nodes"] = std::move(v_nodesArray);
	}

	{
		nlohmann::json v_accessorArray = nlohmann::json::array();

		for (const auto& v_curAccessor : v_writerContext.m_vecBufferAccessors)
		{
			nlohmann::json v_curAccessorJson = nlohmann::json::object();
			v_curAccessorJson["componentType"] = static_cast<std::uint32_t>(v_curAccessor.m_componentType);
			v_curAccessorJson["byteOffset"] = v_curAccessor.m_bufferOffset;
			v_curAccessorJson["bufferView"] = v_curAccessor.m_bufferViewIdx;
			v_curAccessorJson["count"] = v_curAccessor.m_itemCount;
			v_curAccessorJson["type"] = GltfAccessorTypeToString(v_curAccessor.m_type);

			v_accessorArray.push_back(std::move(v_curAccessorJson));
		}

		v_root["accessors"] = std::move(v_accessorArray);
	}

	{
		nlohmann::json v_meshesArray = nlohmann::json::array();

		for (const auto& v_curMesh : v_writerContext.m_vecMeshes)
		{
			nlohmann::json v_curMeshJson = nlohmann::json::object();
			v_curMeshJson["name"] = "Test";

			{
				nlohmann::json v_primitivesArray = nlohmann::json::array();

				{
					nlohmann::json v_primitiveObject = nlohmann::json::object();
					v_primitiveObject["indices"] = v_curMesh.m_indexAccessorIdx;
					v_primitiveObject["material"] = 0;

					{
						nlohmann::json v_attributesObject = nlohmann::json::object();
						
						if (v_curMesh.m_vertexAccessorIdx != std::size_t(-1))
							v_attributesObject["POSITION"] = v_curMesh.m_vertexAccessorIdx;

						if (v_curMesh.m_uvAccessorIdx != std::size_t(-1))
							v_attributesObject["TEXTURE_0"] = v_curMesh.m_uvAccessorIdx;

						if (v_curMesh.m_normalAccessorIdx != std::size_t(-1))
							v_attributesObject["NORMAL"] = v_curMesh.m_normalAccessorIdx;

						v_primitiveObject["attributes"] = std::move(v_attributesObject);
					}

					v_primitivesArray.push_back(std::move(v_primitiveObject));
				}

				v_curMeshJson["primitives"] = std::move(v_primitivesArray);
			}

			v_meshesArray.push_back(std::move(v_curMeshJson));
		}

		v_root["meshes"] = std::move(v_meshesArray);
	}

	{
		nlohmann::json v_bufferViewArray = nlohmann::json::array();

		for (const auto& v_curView : v_writerContext.m_vecBufferViews)
		{
			nlohmann::json v_curViewObject = nlohmann::json::object();
			v_curViewObject["buffer"] = 0;
			v_curViewObject["byteLength"] = v_curView.m_byteLength;
			v_curViewObject["byteOffset"] = v_curView.m_byteOffset;
			v_curViewObject["target"] = v_curView.m_target;

			v_bufferViewArray.push_back(std::move(v_curViewObject));
		}

		v_root["bufferViews"] = std::move(v_bufferViewArray);
	}

	{
		nlohmann::json v_bufferArray = nlohmann::json::array();

		nlohmann::json v_bufferObject = nlohmann::json::object();
		v_bufferObject["byteLength"] = v_writerContext.m_bytesWritten;
		v_bufferObject["uri"] = "mesh_data.bin";

		v_bufferArray.push_back(std::move(v_bufferObject));

		v_root["buffers"] = std::move(v_bufferArray);
	}

	std::ofstream v_gltfOutput(v_bpOutputPath + L".gltf");
	if (v_gltfOutput.is_open())
		v_gltfOutput << std::setfill('\t') << std::setw(1) << v_root;
#endif
}

SMBody* BlueprintConv::CreateCollection(
	SMBlueprint* self,
	const std::string_view& name)
{
	const auto v_iter = BlueprintConv::BodyGroupMap.find(name);
	if (v_iter != BlueprintConv::BodyGroupMap.end())
		return v_iter->second;

	SMBody* v_newCollection = new SMBody(name);

	DebugOutL("Created a new collection: ", name);

	BlueprintConv::BodyGroupMap.emplace(name, v_newCollection);
	self->m_objects.push_back(v_newCollection);

	return v_newCollection;
}

void BlueprintConv::CreateAndAddObjToCollection(
	SMBlueprint* self,
	const std::string_view& name,
	SMEntity* pEntity)
{
	SMBody* v_collection = BlueprintConv::CreateCollection(self, name);

	v_collection->Add(pEntity);
	BlueprintConv::BodyIndexMap.emplace(pEntity->GetIndex(), v_collection);
}

void BlueprintConv::CreateAndAddObjToCollectionNI(
	SMBlueprint* self,
	const std::string_view& name,
	SMEntity* pEntity)
{
	BlueprintConv::CreateCollection(self, name)->Add(pEntity);
}

void BlueprintConv::BlueprintAddObject_SeparateAll(SMBlueprint* self, SMEntity* pEntity)
{
	if (pEntity->Type() == EntityType::Block)
	{
		SMBlock* v_block = reinterpret_cast<SMBlock*>(pEntity);

		const std::size_t v_bounds_x = static_cast<std::size_t>(v_block->m_size.x);
		const std::size_t v_bounds_y = static_cast<std::size_t>(v_block->m_size.y);
		const std::size_t v_bounds_z = static_cast<std::size_t>(v_block->m_size.z);

		std::string v_collName("Object_");
		String::AppendIntegerToString(v_collName, self->m_objectIndex + 1);

		const std::string_view v_collNameView(v_collName);

		if (v_bounds_x == 1 && v_bounds_y == 1 && v_bounds_z == 1)
		{
			BlueprintConv::CreateAndAddObjToCollectionNI(self, v_collNameView, pEntity);
		}
		else
		{
			SMBody* v_pCollection = BlueprintConv::CreateCollection(self, v_collNameView);
			SMEntity** v_addIter = v_pCollection->ResizeAdd(v_bounds_x * v_bounds_y * v_bounds_z);

			glm::vec3 v_blk_pos;

			for (std::size_t x = 0; x < v_bounds_x; x++)
			{
				v_blk_pos.x = static_cast<float>(x);
				for (std::size_t y = 0; y < v_bounds_y; y++)
				{
					v_blk_pos.y = static_cast<float>(y);
					for (std::size_t z = 0; z < v_bounds_z; z++)
					{
						v_blk_pos.z = static_cast<float>(z);

						(*v_addIter++) = new SMBlock(
							v_block,
							v_block->m_position + v_blk_pos,
							v_block->m_rotation,
							glm::vec3(1.0f)
						);
					}
				}
			}

			delete v_block;
		}
	}
	else
	{
		BlueprintConv::CreateAndAddObjToCollectionNI(self, "Object", pEntity);
	}
}

void BlueprintConv::BlueprintAddObject_SeparateShapes(SMBlueprint* self, SMEntity* pEntity)
{
	std::string v_groupName("Object_");
	String::AppendIntegerToString(v_groupName, self->m_objectIndex + 1);

	BlueprintConv::CreateAndAddObjToCollectionNI(self, v_groupName, pEntity);
}

void BlueprintConv::BlueprintAddObject_SeparateJoints(SMBlueprint* self, SMEntity* pEntity)
{
	if (pEntity->Type() == EntityType::Joint)
	{
		const std::size_t v_child_idx = pEntity->GetIndex();

		const auto v_iter = BlueprintConv::BodyIndexMap.find(v_child_idx);
		if (v_iter != BlueprintConv::BodyIndexMap.end())
		{
			v_iter->second->Add(pEntity);
		}
		else
		{
			BlueprintConv::CreateAndAddObjToCollection(self, "Joints", pEntity);
		}
	}
	else
	{
		std::string v_groupName("Objects_");
		String::AppendIntegerToString(v_groupName, self->m_bodyIndex + 1);

		BlueprintConv::CreateAndAddObjToCollection(self, v_groupName, pEntity);
	}
}

void BlueprintConv::BlueprintAddObject_SeparateUuid(SMBlueprint* self, SMEntity* pEntity)
{
	char v_uuidBuffer[36];
	pEntity->GetUuid().toCString(v_uuidBuffer);

	BlueprintConv::CreateAndAddObjToCollectionNI(self, std::string_view(v_uuidBuffer, sizeof(v_uuidBuffer)), pEntity);
}

void BlueprintConv::BlueprintAddObject_SeparateColor(SMBlueprint* self, SMEntity* pEntity)
{
	char v_colBuffer[6];
	pEntity->GetColor().StringHexCStr(v_colBuffer);

	BlueprintConv::CreateAndAddObjToCollectionNI(self, std::string_view(v_colBuffer, sizeof(v_colBuffer)), pEntity);
}

void BlueprintConv::BlueprintAddObject_SeparateUuidAndColor(SMBlueprint* self, SMEntity* pEntity)
{
	char v_uuidAndColBuffer[36 + 1 + 6];
	pEntity->GetUuid().toCString(v_uuidAndColBuffer);
	v_uuidAndColBuffer[36] = '_';
	pEntity->GetColor().StringHexCStr(v_uuidAndColBuffer + 37);

	BlueprintConv::CreateAndAddObjToCollectionNI(
		self,
		std::string_view(v_uuidAndColBuffer, sizeof(v_uuidAndColBuffer)),
		pEntity
	);
}

SMBlueprint::AddObjectFunction BlueprintConv::GetAddObjectFunction()
{
	switch (BlueprintConverterSettings::SeparationType)
	{
	case BPObjectSep_All:          return BlueprintConv::BlueprintAddObject_SeparateAll;
	case BPObjectSep_Shapes:	   return BlueprintConv::BlueprintAddObject_SeparateShapes;
	case BPObjectSep_Joints:       return BlueprintConv::BlueprintAddObject_SeparateJoints;
	case BPObjectSep_Uuid:         return BlueprintConv::BlueprintAddObject_SeparateUuid;
	case BPObjectSep_Color:        return BlueprintConv::BlueprintAddObject_SeparateColor;
	case BPObjectSep_UuidAndColor: return BlueprintConv::BlueprintAddObject_SeparateUuidAndColor;
	}

	return nullptr;
}

void BlueprintConv::ConvertToModel(
	const std::wstring& bp_path,
	const std::wstring& bp_name,
	ConvertError& error,
	CustomGame* pCustomGame)
{
	if (!File::IsRegularFile(bp_path))
	{
		error.setError(1, "The specified path leads to a directory");
		return;
	}

	SMBlueprint::AddObjectFunction v_add_obj_func = BlueprintConv::GetAddObjectFunction();
	SMBlueprint* v_pBlueprint = nullptr;

	if (pCustomGame)
	{
		SMModCustomGameSwitch<true, false> v_content_switch;
		v_content_switch.MergeContent(pCustomGame);

		v_pBlueprint = SMBlueprint::FromFileWithStatus(bp_path, v_add_obj_func, error);
	}
	else
	{
		v_pBlueprint = SMBlueprint::FromFileWithStatus(bp_path, v_add_obj_func, error);
	}

	if (v_pBlueprint)
	{
		BlueprintConv::WriteToFileInternal(v_pBlueprint, bp_name, error);

		//Since all the sorted groups are attached to the blueprint, the blueprint takes care of all the allocated memory
		delete v_pBlueprint;
	}

	BlueprintConv::BodyIndexMap.clear();
	BlueprintConv::BodyGroupMap.clear();

	ModelStorage::ClearStorage(); //clear the cache
}
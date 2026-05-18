#include "BlueprintConverter.hpp"

#include "ObjectDatabase/Mods/CustomGameSwitch.hpp"
#include "ObjectDatabase/DatabaseConfig.hpp"
#include "ObjectDatabase/ProgCounter.hpp"

#include "Converter/Entity/ControllerParser.hpp"
#include "Converter/Entity/ControllerTransform.hpp"
#include "Converter/ConvertSettings.hpp"
#include "Converter/MtlFileWriter.hpp"

#include "Utils/Console.hpp"
#include "Utils/File.hpp"

SM_UNMANAGED_CODE

void BlueprintConv::WriteToFileInternal(
	SMBlueprint* pBlueprint,
	const std::wstring& blueprintName,
	ConvertError& error)
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

	{
		// Write object file
		std::ofstream v_obj_writer(v_bpOutputPath + L".obj");
		if (!v_obj_writer.is_open())
		{
			error.setError(1, "Couldn't create an object file");
			return;
		}

		glm::vec3 v_centerPoint(0.0f);

		const std::size_t v_bp_object_count = pBlueprint->GetAmountOfObjects();
		if (v_bp_object_count > 0) //prevent division by 0 exception
		{
			pBlueprint->CalculateCenterPoint(v_centerPoint);
			v_centerPoint /= static_cast<float>(v_bp_object_count);
		}

		v_centerPoint *= pBlueprint->m_size;

		ProgCounter::SetState(ProgState::WritingObjects, v_bp_object_count);

		WriterOffsetData v_offset_data;
		pBlueprint->WriteObjectToFile(v_obj_writer, v_offset_data, glm::translate(-v_centerPoint));
		v_obj_writer.close();
	}

	{
		ProgCounter::SetState(ProgState::WritingMtlFile, 0);

		SMEntity::EntityTextureMap v_textureMap;
		pBlueprint->FillTextureMap(v_textureMap);
		ProgCounter::ProgressMax = v_textureMap.size();

		MtlFileWriter::Write(v_bpOutputPath + L".mtl", v_textureMap);
	}
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

void BlueprintConv::BlueprintAddObject_SeparateAll(
	SMBlueprint* self,
	SMEntity* pEntity)
{
	if (pEntity->Type() != EntityType::Block)
	{
		BlueprintConv::CreateAndAddObjToCollectionNI(self, "Object", pEntity);
		return;
	}

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

		glm::vec3 v_blkPos;

		for (std::size_t x = 0; x < v_bounds_x; x++)
		{
			v_blkPos.x = static_cast<float>(x);
			for (std::size_t y = 0; y < v_bounds_y; y++)
			{
				v_blkPos.y = static_cast<float>(y);
				for (std::size_t z = 0; z < v_bounds_z; z++)
				{
					v_blkPos.z = static_cast<float>(z);

					(*v_addIter++) = new SMBlock(
						v_block,
						v_block->m_position + v_blkPos,
						v_block->m_rotation,
						glm::vec3(1.0f)
					);
				}
			}
		}

		delete v_block;
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
	const std::wstring& blueprintPath,
	const std::wstring& blueprintName,
	ConvertError& error,
	CustomGame* pCustomGame)
{
	if (!File::IsRegularFile(blueprintPath))
	{
		error.setError(1, "The specified path leads to a directory");
		return;
	}

	SMBlueprint::AddObjectFunction v_add_obj_func = BlueprintConv::GetAddObjectFunction();
	SMBlueprint* v_pBlueprint = nullptr;

	// Reuse parsed DOM for controller presets to avoid double-parsing
	const bool v_needPresets = BlueprintConverterSettings::ApplyControllerPresets;
	simdjson::dom::document v_bpDoc;
	simdjson::dom::document* v_pDocOut = v_needPresets ? &v_bpDoc : nullptr;

	if (pCustomGame)
	{
		SMModCustomGameSwitch<true, false> v_content_switch;
		v_content_switch.MergeContent(pCustomGame);

		v_pBlueprint = SMBlueprint::FromFileWithStatus(blueprintPath, v_add_obj_func, error, v_pDocOut);
	}
	else
	{
		v_pBlueprint = SMBlueprint::FromFileWithStatus(blueprintPath, v_add_obj_func, error, v_pDocOut);
	}

	if (v_pBlueprint)
	{
		if (v_needPresets && !error)
		{
			const auto v_bpRoot = v_bpDoc.root();
			ControllerPresetData v_presetData = ControllerParser::ExtractPresets(v_bpRoot);
			auto v_bodyGraph = ControllerParser::BuildBodyGraph(
				v_bpRoot, v_presetData, v_pBlueprint->m_childToBodyIndex);
			ControllerTransform::Apply(v_pBlueprint, v_bodyGraph, v_pBlueprint->m_childToBodyIndex);
		}

		BlueprintConv::WriteToFileInternal(v_pBlueprint, blueprintName, error);

		// Since all the sorted groups are attached to the blueprint, the blueprint takes care of all the allocated memory
		delete v_pBlueprint;
	}

	BlueprintConv::BodyIndexMap.clear();
	BlueprintConv::BodyGroupMap.clear();

	ModelStorage::ClearStorage(); //clear the cache
}
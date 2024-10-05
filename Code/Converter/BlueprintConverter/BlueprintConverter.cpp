#include "BlueprintConverter.hpp"

#include "ObjectDatabase\Mods\CustomGameSwitch.hpp"
#include "ObjectDatabase\DatabaseConfig.hpp"
#include "ObjectDatabase\ProgCounter.hpp"

#include "Converter\ConvertSettings.hpp"
#include "Converter\MtlFileWriter.hpp"

#include "Utils\Console.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

void BlueprintConv::WriteToFileInternal(SMBlueprint* pBlueprint, const std::wstring& bp_name, ConvertError& error)
{
	if (error) return;

	const std::wstring v_bp_out_dir = std::wstring(DatabaseConfig::BlueprintOutputFolder.data());
	if (!File::CreateDirectorySafe(v_bp_out_dir))
	{
		error.setError(1, L"Couldn't create the main output directory");
		return;
	}

	const std::wstring v_bp_dir_path = v_bp_out_dir + L"/" + bp_name;
	if (!File::CreateDirectorySafe(v_bp_dir_path))
	{
		error.setError(1, L"Couldn't create the blueprint output directory");
		return;
	}

	const std::wstring v_bp_output_path = v_bp_dir_path + L"/" + bp_name;

	{
		//Write object file
		std::ofstream v_obj_writer(v_bp_output_path + L".obj");
		if (!v_obj_writer.is_open())
		{
			error.setError(1, L"Couldn't create an object file");
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

		std::unordered_map<std::string, ObjectTexData> v_tex_map;

		for (const SMEntity* v_entity : pBlueprint->Objects)
		{
			v_entity->FillTextureMap(v_tex_map);
			ProgCounter::ProgressMax = v_tex_map.size();
		}

		MtlFileWriter::Write(v_bp_output_path + L".mtl", v_tex_map);
	}
}

SMBody* BlueprintConv::CreateCollection(SMBlueprint* self, const std::string& name)
{
	const auto v_iter = BlueprintConv::BodyGroupMap.find(name);
	if (v_iter != BlueprintConv::BodyGroupMap.end())
		return v_iter->second;

	SMBody* v_new_collection = new SMBody(name);

	DebugOutL("Created a new collection: ", name);

	BlueprintConv::BodyGroupMap.emplace(name, v_new_collection);
	self->Objects.push_back(v_new_collection);

	return v_new_collection;
}

void BlueprintConv::CreateAndAddObjToCollection(SMBlueprint* self, const std::string& name, SMEntity* pEntity)
{
	SMBody* v_collection = BlueprintConv::CreateCollection(self, name);

	v_collection->Add(pEntity);
	BlueprintConv::BodyIndexMap.emplace(pEntity->GetIndex(), v_collection);
}

void BlueprintConv::CreateAndAddObjToCollectionNI(SMBlueprint* self, const std::string& name, SMEntity* pEntity)
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

		const std::string v_coll_name = "Object_" + std::to_string(self->m_object_index + 1);

		if (v_bounds_x == 1 && v_bounds_y == 1 && v_bounds_z == 1)
		{
			BlueprintConv::CreateAndAddObjToCollectionNI(self, v_coll_name, pEntity);
		}
		else
		{
			SMBody* v_pCollection = BlueprintConv::CreateCollection(self, v_coll_name);
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

						SMBlock* v_new_blk = new SMBlock(v_block, v_block->m_position + v_blk_pos,
							v_block->m_rotation, glm::vec3(1.0f));

						v_pCollection->Add(v_new_blk);
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
	BlueprintConv::CreateAndAddObjToCollectionNI(self, "Object_" + std::to_string(self->m_object_index + 1), pEntity);
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
		BlueprintConv::CreateAndAddObjToCollection(self, "Objects_" + std::to_string(self->m_body_index + 1), pEntity);
	}
}

void BlueprintConv::BlueprintAddObject_SeparateUuid(SMBlueprint* self, SMEntity* pEntity)
{
	BlueprintConv::CreateAndAddObjToCollectionNI(self, pEntity->GetUuid().ToString(), pEntity);
}

void BlueprintConv::BlueprintAddObject_SeparateColor(SMBlueprint* self, SMEntity* pEntity)
{
	BlueprintConv::CreateAndAddObjToCollectionNI(self, pEntity->GetColor().StringHex(), pEntity);
}

void BlueprintConv::BlueprintAddObject_SeparateUuidAndColor(SMBlueprint* self, SMEntity* pEntity)
{
	std::string v_group_name = pEntity->GetUuid().ToString();
	v_group_name.append("_");
	v_group_name.append(pEntity->GetColor().StringHex());

	BlueprintConv::CreateAndAddObjToCollectionNI(self, v_group_name, pEntity);
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
		error.setError(1, L"The specified path leads to a directory");
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
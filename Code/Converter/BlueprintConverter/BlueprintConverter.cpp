#include "BlueprintConverter.hpp"

#include "ObjectDatabase\Mods\CustomGameSwitch.hpp"
#include "ObjectDatabase\DatabaseConfig.hpp"
#include "ObjectDatabase\ProgCounter.hpp"

#include "Converter\ConvertSettings.hpp"
#include "Converter\MtlFileWriter.hpp"

#include "Utils\Console.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

void BlueprintConv::WriteToFileInternal(SMBlueprint* blueprint, const std::wstring& bp_name, ConvertError& v_error)
{
	if (v_error) return;

	const std::wstring v_bp_out_dir = std::wstring(DatabaseConfig::BlueprintOutputFolder.data());
	if (!File::CreateDirectorySafe(v_bp_out_dir))
	{
		v_error = ConvertError(1, L"Couldn't create the main output directory");
		return;
	}

	const std::wstring v_bp_dir_path = v_bp_out_dir + L"/" + bp_name;
	if (!File::CreateDirectorySafe(v_bp_dir_path))
	{
		v_error = ConvertError(1, L"Couldn't create the blueprint output directory");
		return;
	}

	const std::wstring v_bp_output_path = v_bp_dir_path + L"/" + bp_name;

	{
		//Write object file
		std::ofstream v_obj_writer(v_bp_output_path + L".obj");
		if (!v_obj_writer.is_open())
		{
			v_error = ConvertError(1, L"Couldn't create an object file");
			return;
		}

		glm::vec3 v_center_point(0.0f);

		const std::size_t v_bp_object_count = blueprint->GetAmountOfObjects();
		if (v_bp_object_count > 0) //prevent division by 0 exception
		{
			blueprint->CalculateCenterPoint(v_center_point);
			v_center_point /= static_cast<float>(v_bp_object_count);
		}

		v_center_point *= blueprint->m_size;

		ProgCounter::SetState(ProgState::WritingObjects, v_bp_object_count);

		WriterOffsetData v_offset_data;
		blueprint->WriteObjectToFile(v_obj_writer, v_offset_data, glm::translate(-v_center_point));
		v_obj_writer.close();
	}

	{
		ProgCounter::SetState(ProgState::WritingMtlFile, 0);

		std::unordered_map<std::string, ObjectTexData> v_tex_map;

		for (const SMEntity* v_entity : blueprint->Objects)
		{
			v_entity->FillTextureMap(v_tex_map);
			ProgCounter::ProgressMax = v_tex_map.size();
		}

		MtlFileWriter::Write(v_bp_output_path + L".mtl", v_tex_map);
	}
}

SMBody* BlueprintConv::CreateCollection(SMBlueprint* self, const std::string& v_name)
{
	const auto v_iter = BlueprintConv::BodyGroupMap.find(v_name);
	if (v_iter != BlueprintConv::BodyGroupMap.end())
		return v_iter->second;

	SMBody* v_new_collection = new SMBody(v_name);

	DebugOutL("Created a new collection: ", v_name);

	BlueprintConv::BodyGroupMap.emplace(v_name, v_new_collection);
	self->Objects.push_back(v_new_collection);

	return v_new_collection;
}

void BlueprintConv::CreateAndAddObjToCollection(SMBlueprint* self, const std::string& v_name, SMEntity* v_entity)
{
	SMBody* v_collection = BlueprintConv::CreateCollection(self, v_name);

	v_collection->Add(v_entity);
	BlueprintConv::BodyIndexMap.emplace(v_entity->GetIndex(), v_collection);
}

void BlueprintConv::CreateAndAddObjToCollectionNI(SMBlueprint* self, const std::string& v_name, SMEntity* v_entity)
{
	BlueprintConv::CreateCollection(self, v_name)->Add(v_entity);
}

void BlueprintConv::BlueprintAddObject_SeparateAll(SMBlueprint* self, SMEntity* v_entity)
{
	if (v_entity->Type() == EntityType::Block)
	{
		SMBlock* v_block = reinterpret_cast<SMBlock*>(v_entity);

		const std::size_t v_bounds_x = static_cast<std::size_t>(v_block->m_size.x);
		const std::size_t v_bounds_y = static_cast<std::size_t>(v_block->m_size.y);
		const std::size_t v_bounds_z = static_cast<std::size_t>(v_block->m_size.z);

		const std::string v_coll_name = "Object_" + std::to_string(self->m_object_index + 1);

		if (v_bounds_x == 1 && v_bounds_y == 1 && v_bounds_z == 1)
		{
			BlueprintConv::CreateAndAddObjToCollectionNI(self, v_coll_name, v_entity);
		}
		else
		{
			SMBody* v_collection = BlueprintConv::CreateCollection(self, v_coll_name);
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

						v_collection->Add(v_new_blk);
					}
				}
			}

			delete v_block;
		}
	}
	else
	{
		BlueprintConv::CreateAndAddObjToCollectionNI(self, "Object", v_entity);
	}
}

void BlueprintConv::BlueprintAddObject_SeparateShapes(SMBlueprint* self, SMEntity* v_entity)
{
	BlueprintConv::CreateAndAddObjToCollectionNI(self, "Object_" + std::to_string(self->m_object_index + 1), v_entity);
}

void BlueprintConv::BlueprintAddObject_SeparateJoints(SMBlueprint* self, SMEntity* v_entity)
{
	if (v_entity->Type() == EntityType::Joint)
	{
		const std::size_t v_child_idx = v_entity->GetIndex();

		const auto v_iter = BlueprintConv::BodyIndexMap.find(v_child_idx);
		if (v_iter != BlueprintConv::BodyIndexMap.end())
		{
			v_iter->second->Add(v_entity);
		}
		else
		{
			BlueprintConv::CreateAndAddObjToCollection(self, "Joints", v_entity);
		}
	}
	else
	{
		BlueprintConv::CreateAndAddObjToCollection(self, "Objects_" + std::to_string(self->m_body_index + 1), v_entity);
	}
}

void BlueprintConv::BlueprintAddObject_SeparateUuid(SMBlueprint* self, SMEntity* v_entity)
{
	BlueprintConv::CreateAndAddObjToCollectionNI(self, v_entity->GetUuid().ToString(), v_entity);
}

void BlueprintConv::BlueprintAddObject_SeparateColor(SMBlueprint* self, SMEntity* v_entity)
{
	BlueprintConv::CreateAndAddObjToCollectionNI(self, v_entity->GetColor().StringHex(), v_entity);
}

void BlueprintConv::BlueprintAddObject_SeparateUuidAndColor(SMBlueprint* self, SMEntity* v_entity)
{
	std::string v_group_name = v_entity->GetUuid().ToString();
	v_group_name.append("_");
	v_group_name.append(v_entity->GetColor().StringHex());

	BlueprintConv::CreateAndAddObjToCollectionNI(self, v_group_name, v_entity);
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

void BlueprintConv::ConvertToModel(const std::wstring& bp_path, const std::wstring& bp_name, ConvertError& v_error, CustomGame* v_custom_game)
{
	if (!File::IsRegularFile(bp_path))
	{
		v_error = ConvertError(1, L"The specified path leads to a directory");
		return;
	}

	SMBlueprint::AddObjectFunction v_add_obj_func = BlueprintConv::GetAddObjectFunction();
	SMBlueprint* v_blueprint = nullptr;

	if (v_custom_game)
	{
		SMModCustomGameSwitch<true, false> v_content_switch;
		v_content_switch.MergeContent(v_custom_game);

		v_blueprint = SMBlueprint::FromFileWithStatus(bp_path, v_add_obj_func, v_error);
	}
	else
	{
		v_blueprint = SMBlueprint::FromFileWithStatus(bp_path, v_add_obj_func, v_error);
	}

	if (v_blueprint)
	{
		BlueprintConv::WriteToFileInternal(v_blueprint, bp_name, v_error);

		//Since all the sorted groups are attached to the blueprint, the blueprint takes care of all the allocated memory
		delete v_blueprint;
	}

	BlueprintConv::BodyIndexMap.clear();
	BlueprintConv::BodyGroupMap.clear();

	ModelStorage::ClearStorage(); //clear the cache
}
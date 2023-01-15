#include "BlueprintConverter.hpp"

#include "ObjectDatabase\ProgCounter.hpp"

#include "Converter\ConvertSettings.hpp"
#include "Converter\MtlFileWriter.hpp"

#include "Utils\Console.hpp"
#include "Utils\File.hpp"

constexpr const static std::wstring_view g_BlueprintOutputDirectory = L"./ConvertedBlueprints";

void BlueprintConv::WriteToFileInternal(Blueprint* blueprint, const std::wstring& bp_name, ConvertError& v_error)
{
	if (v_error) return;

	const std::wstring v_bp_out_dir = std::wstring(g_BlueprintOutputDirectory.data());
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

		ProgCounter::SetState(ProgState::WritingObjects, blueprint->GetAmountOfObjects());

		WriterOffsetData v_offset_data;
		blueprint->WriteObjectToFile(v_obj_writer, v_offset_data, glm::mat4(1.0f));
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

void BlueprintConv::CreateAndAddObjToCollection(Blueprint* self, const std::string& v_name, SMEntity* v_entity)
{
	SMBody* v_collection = nullptr;

	const auto v_iter = BlueprintConv::BodyGroupMap.find(v_name);
	if (v_iter != BlueprintConv::BodyGroupMap.end())
	{
		v_collection = v_iter->second;
	}
	else
	{
		v_collection = new SMBody(v_name);

		DebugOutL("Created a new collection: ", v_name);

		BlueprintConv::BodyGroupMap.insert(std::make_pair(v_name, v_collection));
		self->Objects.push_back(v_collection);
	}

	v_collection->Add(v_entity);
	BlueprintConv::BodyIndexMap.insert(std::make_pair(v_entity->GetIndex(), v_collection));
}

void BlueprintConv::BlueprintAddObject_SeparateAll(Blueprint* self, SMEntity* v_entity)
{
	BlueprintConv::CreateAndAddObjToCollection(self, "Object_" + std::to_string(self->m_object_index + 1), v_entity);
}

void BlueprintConv::BlueprintAddObject_SeparateJoints(Blueprint* self, SMEntity* v_entity)
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

void BlueprintConv::BlueprintAddObject_SeparateUuid(Blueprint* self, SMEntity* v_entity)
{
	BlueprintConv::CreateAndAddObjToCollection(self, v_entity->GetUuid().ToString(), v_entity);
}

void BlueprintConv::BlueprintAddObject_SeparateColor(Blueprint* self, SMEntity* v_entity)
{
	BlueprintConv::CreateAndAddObjToCollection(self, v_entity->GetColor().StringHex(), v_entity);
}

void BlueprintConv::BlueprintAddObject_SeparateUuidAndColor(Blueprint* self, SMEntity* v_entity)
{
	std::string v_group_name = v_entity->GetUuid().ToString();
	v_group_name.append("_");
	v_group_name.append(v_entity->GetColor().StringHex());

	BlueprintConv::CreateAndAddObjToCollection(self, v_group_name, v_entity);
}

Blueprint::AddObjectFunction BlueprintConv::GetAddObjectFunction()
{
	switch (BlueprintConverterSettings::SeparationType)
	{
	case BPObjectSep_All:          return BlueprintConv::BlueprintAddObject_SeparateAll;
	case BPObjectSep_Joints:       return BlueprintConv::BlueprintAddObject_SeparateJoints;
	case BPObjectSep_Uuid:         return BlueprintConv::BlueprintAddObject_SeparateUuid;
	case BPObjectSep_Color:        return BlueprintConv::BlueprintAddObject_SeparateColor;
	case BPObjectSep_UuidAndColor: return BlueprintConv::BlueprintAddObject_SeparateUuidAndColor;
	}

	return nullptr;
}

void BlueprintConv::ConvertToModel(const std::wstring& bp_path, const std::wstring& bp_name, ConvertError& v_error)
{
	if (!File::IsRegularFile(bp_path))
	{
		v_error = ConvertError(1, L"The specified path leads to a directory");
		return;
	}

	Blueprint::AddObjectFunction v_add_obj_func = BlueprintConv::GetAddObjectFunction();

	Blueprint* v_blueprint = Blueprint::FromFileWithStatus(bp_path, v_add_obj_func, v_error);
	if (v_blueprint)
	{
		BlueprintConv::WriteToFileInternal(v_blueprint, bp_name, v_error);

		delete v_blueprint;
	}

	BlueprintConv::BodyIndexMap.clear();
	BlueprintConv::BodyGroupMap.clear();

	ModelStorage::ClearStorage(); //clear the cache
}
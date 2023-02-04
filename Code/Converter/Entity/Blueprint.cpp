#include "Blueprint.hpp"

#include "ObjectDatabase\UserDataReaders\ItemModCounter.hpp"
#include "ObjectDatabase\KeywordReplacer.hpp"
#include "ObjectDatabase\ObjectRotations.hpp"
#include "ObjectDatabase\ProgCounter.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\GlmUnmanaged.hpp"
#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

void SMBlueprint::LoadAndCountAutomatic(const std::string& str)
{
	const std::size_t v_secret_idx = str.find("?JB:");
	if (v_secret_idx != std::string::npos)
	{
		const std::string v_bp_str = str.substr(v_secret_idx + 4);

		DebugOutL(0b0101_fg, "CountingBlueprintJsonString: ", v_bp_str);

		SMBlueprint::CountFromJsonString(v_bp_str);
		return;
	}

	const std::wstring v_wide_str = String::ToWide(str);
	const std::wstring v_bp_path = KeywordReplacer::ReplaceKey(v_wide_str);

	DebugOutL(0b0011_fg, "CountingBlueprintPath: ", v_bp_path);

	SMBlueprint::CountFromFile(v_bp_path);
}

void SMBlueprint::CountFromFile(const std::wstring& path)
{
	std::string v_file_str;
	if (!File::ReadToString(path, v_file_str))
	{
		DebugErrorL("Couldn't count the specified blueprint: ", path);
		return;
	}

	return SMBlueprint::CountFromJsonString(v_file_str);
}

void SMBlueprint::CountFromJsonString(const std::string& str)
{
	simdjson::dom::document v_bp_doc;
	if (!JsonReader::ParseSimdjsonString(str, v_bp_doc))
		return;

	const auto v_root = v_bp_doc.root();
	if (!v_root.is_object()) return;

	const auto v_bodies_obj = v_root["bodies"];
	if (v_bodies_obj.is_array())
	{
		for (const auto v_body : v_bodies_obj.get_array())
		{
			const auto v_childs_obj = v_body["childs"];
			if (!v_childs_obj.is_array()) continue;

			for (const auto v_child : v_childs_obj.get_array())
			{
				if (!v_child.is_object()) continue;

				const auto v_uuid_obj = v_child["shapeId"];
				if (!v_uuid_obj.is_string()) continue;

				const SMUuid v_uuid = v_uuid_obj.get_c_str().value_unsafe();
				SMMod* v_cur_mod = SMMod::GetModFromBlocksAndParts(v_uuid);
				ItemModStats::IncrementModPart(v_cur_mod);
			}
		}
	}

	const auto v_joints_obj = v_root["joints"];
	if (v_joints_obj.is_array())
	{
		for (const auto v_joint : v_joints_obj.get_array())
		{
			if (!v_joint.is_object()) continue;

			const auto v_uuid_obj = v_joint["shapeId"];
			if (!v_uuid_obj.is_string()) continue;

			const SMUuid v_uuid = v_uuid_obj.get_c_str().value_unsafe();
			SMMod* v_cur_mod = SMMod::GetModFromBlocksAndParts<false>(v_uuid);
			ItemModStats::IncrementModPart(v_cur_mod);
		}
	}
}

SMBlueprint* SMBlueprint::LoadAutomatic(const std::string& str)
{
	const std::size_t secret_idx = str.find("?JB:");
	if (secret_idx != std::string::npos)
	{
		const std::string bp_str = str.substr(secret_idx + 4);

		DebugOutL(0b0101_fg, "LoadingBlueprintJsonString: ", bp_str);

		return SMBlueprint::FromJsonString(bp_str);
	}

	const std::wstring wide_str = String::ToWide(str);
	const std::wstring bp_path = KeywordReplacer::ReplaceKey(wide_str);

	DebugOutL(0b0011_fg, "LoadingBlueprintPath: ", bp_path);

	return SMBlueprint::FromFile(bp_path);
}

SMBlueprint* SMBlueprint::FromFile(const std::wstring& path)
{
	std::string v_fileString;
	if (!File::ReadToString(path, v_fileString))
	{
		DebugErrorL("Couldn't load the specified blueprint: ", path);
		return nullptr;
	}

	return SMBlueprint::FromJsonString(v_fileString);
}

SMBlueprint* SMBlueprint::FromFileWithStatus(const std::wstring& path, SMBlueprint::AddObjectFunction v_addObjFunc, ConvertError& v_error)
{
	ProgCounter::SetState(ProgState::ParsingBlueprint, 0);

	simdjson::dom::document v_bp_doc;
	if (!JsonReader::LoadParseSimdjsonC(path, v_bp_doc, simdjson::dom::element_type::OBJECT))
	{
		v_error = ConvertError(1, L"Couldn't read the specified blueprint file. Possible reason: Invalid file, Parse error, Invalid path");
		return nullptr;
	}

	const auto v_root = v_bp_doc.root();

	const bool has_bodies = v_root["bodies"].error() == simdjson::SUCCESS;
	const bool has_joints = v_root["joints"].error() == simdjson::SUCCESS;

	if (!has_bodies && !has_joints)
	{
		v_error = ConvertError(1, L"The specified blueprint has no objects to convert");
		return nullptr;
	}

	SMBlueprint* v_blueprint = new SMBlueprint();

	//Bind the custom function if it has been passed
	if (v_addObjFunc)
		v_blueprint->m_addObjectFunction = v_addObjFunc;

	v_blueprint->LoadBodiesWithCounter(v_root);
	v_blueprint->LoadJointsWithCounter(v_root);

	return v_blueprint;
}

SMBlueprint* SMBlueprint::FromJsonString(const std::string& json_str)
{
	simdjson::dom::document v_bp_doc;
	if (!JsonReader::ParseSimdjsonString(json_str, v_bp_doc))
		return nullptr;

	const auto v_root = v_bp_doc.root();
	if (!v_root.is_object())
		return nullptr;

	SMBlueprint* nBlueprint = new SMBlueprint();

	nBlueprint->LoadBodies(v_root);
	nBlueprint->LoadJoints(v_root);

	return nBlueprint;
}

void SMBlueprint::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	for (const SMEntity* pEntity : this->Objects)
		pEntity->FillTextureMap(tex_map);
}

void SMBlueprint::WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const
{
	const glm::mat4 blueprint_matrix = transform_matrix * this->GetTransformMatrix();

	for (const SMEntity* pEntity : this->Objects)
		pEntity->WriteObjectToFile(file, mOffset, blueprint_matrix);
}

std::size_t SMBlueprint::GetAmountOfObjects() const
{
	std::size_t v_output = 0;

	for (const SMEntity* v_entity : this->Objects)
		v_output += v_entity->GetAmountOfObjects();

	return v_output;
}

void SMBlueprint::CalculateCenterPoint(glm::vec3& v_input) const
{
	for (const SMEntity* v_entity : this->Objects)
		v_entity->CalculateCenterPoint(v_input);
}

void SMBlueprint::AddObject_Default(SMBlueprint* self, SMEntity* v_entity)
{
	self->Objects.push_back(v_entity);
}

glm::vec3 SMBlueprint::JsonToVector(const simdjson::simdjson_result<simdjson::dom::element>& vec_json)
{
	if (vec_json.is_object())
	{
		const auto v_x = vec_json["x"];
		const auto v_y = vec_json["y"];
		const auto v_z = vec_json["z"];

		if (v_x.is_number() && v_y.is_number() && v_z.is_number())
			return glm::vec3(JsonReader::GetNumber<float>(v_x), JsonReader::GetNumber<float>(v_y), JsonReader::GetNumber<float>(v_z));
	}

	return glm::vec3(0.0f, 0.0f, 0.0f);
}

void SMBlueprint::LoadChild(const simdjson::dom::element& v_child)
{
	if (!v_child.is_object()) return;

	const auto v_uuid = v_child["shapeId"];
	const auto v_position = v_child["pos"];
	const auto v_x_axis = v_child["xaxis"];
	const auto v_z_axis = v_child["zaxis"];
	const auto v_bounds = v_child["bounds"];
	const auto v_color = v_child["color"];

	if (!(v_uuid.is_string() && v_color.is_string())) return;

	const int v_xAxisInt = (v_x_axis.is_number() ? JsonReader::GetNumber<int>(v_x_axis) : 1);
	const int v_zAxisInt = (v_z_axis.is_number() ? JsonReader::GetNumber<int>(v_z_axis) : 3);

	const glm::vec3 v_obj_pos = SMBlueprint::JsonToVector(v_position);
	const SMUuid v_obj_uuid = v_uuid.get_c_str();
	const SMColor v_obj_color = v_color.get_c_str();

	if (v_bounds.is_object())
	{
		const glm::vec3 v_blk_bounds = SMBlueprint::JsonToVector(v_bounds);

		if (!(v_blk_bounds.x > 0.0f && v_blk_bounds.y > 0.0f && v_blk_bounds.z > 0.0f))
			return;

		BlockData* v_blk_data = SMMod::GetGlobalObject<BlockData>(v_obj_uuid);
		if (!v_blk_data) return;

		SMBlock* v_new_blk = new SMBlock(v_blk_data, v_blk_bounds, v_obj_color, v_xAxisInt, v_zAxisInt, m_object_index);
		v_new_blk->SetPosition(v_obj_pos);
		v_new_blk->SetSize(v_blk_bounds);

		this->m_addObjectFunction(this, v_new_blk);
	}
	else
	{
		PartData* v_prt_data = SMMod::GetGlobalObject<PartData>(v_obj_uuid);
		if (!v_prt_data) return;

		Model* v_prt_model = ModelStorage::LoadModel(v_prt_data->m_mesh);
		if (!v_prt_model) return;

		SMPart* v_new_prt = new SMPart(v_prt_data, v_prt_model, v_obj_color, v_xAxisInt, v_zAxisInt, m_object_index);
		v_new_prt->SetPosition(v_obj_pos);

		this->m_addObjectFunction(this, v_new_prt);
	}
}

void SMBlueprint::LoadJoint(const simdjson::dom::element& v_jnt)
{
	const auto v_x_axis = v_jnt["xaxisA"];
	const auto v_z_axis = v_jnt["zaxisA"];
	const auto v_uuid = v_jnt["shapeId"];
	const auto v_color = v_jnt["color"];
	const auto v_position = v_jnt["posA"];
	const auto v_child = v_jnt["childA"];

	if (!(v_uuid.is_string() && v_color.is_string() && v_child.is_number()))
		return;

	const int v_xAxisInt = (v_x_axis.is_number() ? JsonReader::GetNumber<int>(v_x_axis) : 1);
	const int v_zAxisInt = (v_z_axis.is_number() ? JsonReader::GetNumber<int>(v_z_axis) : 3);

	const glm::vec3 v_pos_vec = SMBlueprint::JsonToVector(v_position);

	const SMUuid v_jnt_uuid = v_uuid.get_c_str();
	const SMColor v_jnt_color = v_color.get_c_str();
	const std::size_t v_child_idx = JsonReader::GetNumber<std::size_t>(v_child);

	PartData* v_jnt_data = SMMod::GetGlobalObject<PartData>(v_jnt_uuid);
	if (!v_jnt_data) return;

	Model* v_jnt_model = ModelStorage::LoadModel(v_jnt_data->m_mesh);
	if (!v_jnt_model) return;

	SMJoint* v_new_jnt = new SMJoint(v_jnt_data, v_jnt_model, v_jnt_color, v_xAxisInt, v_zAxisInt, v_child_idx);
	v_new_jnt->SetPosition(v_pos_vec);

	this->m_addObjectFunction(this, v_new_jnt);
}

void SMBlueprint::LoadBodiesWithCounter(const simdjson::dom::element& v_blueprint)
{
	const auto v_body_array = v_blueprint["bodies"];
	if (!v_body_array.is_array()) return;

	ProgCounter::SetState(ProgState::ReadingBlueprintParts, 0);

	for (const auto v_body : v_body_array.get_array())
	{
		const auto v_childs_obj = v_body["childs"];
		if (!v_childs_obj.is_array()) continue;

		const auto v_childs_array = v_childs_obj.get_array();
		ProgCounter::ProgressMax += v_childs_array.size();

		for (const auto v_child : v_childs_array)
		{
			this->LoadChild(v_child);

			ProgCounter::ProgressValue++;
			m_object_index++;
		}

		m_body_index++;
	}
}

void SMBlueprint::LoadJointsWithCounter(const simdjson::dom::element& v_blueprint)
{
	const auto v_jnt_obj = v_blueprint["joints"];
	if (!v_jnt_obj.is_array())
		return;

	const auto v_jnt_array = v_jnt_obj.get_array();
	ProgCounter::SetState(ProgState::ReadingBlueprintJoints, v_jnt_array.size());

	for (const auto v_jnt : v_jnt_array)
	{
		this->LoadJoint(v_jnt);

		ProgCounter::ProgressValue++;
		m_object_index++;
	}
}

void SMBlueprint::LoadBodies(const simdjson::dom::element& pJson)
{
	const auto v_body_array = pJson["bodies"];
	if (!v_body_array.is_array()) return;

	for (const auto v_body : v_body_array.get_array())
	{
		const auto v_childs_array = v_body["childs"];
		if (!v_childs_array.is_array()) continue;

		for (const auto v_child : v_childs_array)
		{
			this->LoadChild(v_child);
			m_object_index++;
		}

		m_body_index++;
	}
}

void SMBlueprint::LoadJoints(const simdjson::dom::element& pJson)
{
	const auto v_jnt_array = pJson["joints"];
	if (!v_jnt_array.is_array())
		return;

	for (const auto v_jnt : v_jnt_array)
	{
		this->LoadJoint(v_jnt);

		m_object_index++;
	}
}
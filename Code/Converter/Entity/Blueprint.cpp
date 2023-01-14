#include "Blueprint.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"
#include "ObjectDatabase\ObjectRotations.hpp"
#include "ObjectDatabase\ProgCounter.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\File.hpp"

#include <gtx\quaternion.hpp>

static const std::string bp_secret = "?JB:";

Blueprint* Blueprint::LoadAutomatic(const std::string& str)
{
	const std::size_t secret_idx = str.find(bp_secret);
	if (secret_idx != std::string::npos)
	{
		const std::string bp_str = str.substr(secret_idx + bp_secret.size());

		DebugOutL(0b0101_fg, "LoadingBlueprintJsonString: ", bp_str);

		return Blueprint::FromJsonString(bp_str);
	}
	else
	{
		const std::wstring wide_str = String::ToWide(str);
		const std::wstring bp_path = KeywordReplacer::ReplaceKey(wide_str);

		DebugOutL(0b0011_fg, "LoadingBlueprintPath: ", bp_path);

		return Blueprint::FromFile(bp_path);
	}

	return nullptr;
}

Blueprint* Blueprint::FromFile(const std::wstring& path)
{
	std::string v_fileString;
	if (!File::ReadToString(path, v_fileString))
	{
		DebugErrorL("Couldn't load the specified blueprint: ", path);
		return nullptr;
	}

	return Blueprint::FromJsonString(v_fileString);
}

Blueprint* Blueprint::FromFileErrorChecked(const std::wstring& path, void (*v_addObjFunc)(Blueprint*, SMEntity*), ConvertError& v_error)
{
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

	Blueprint* v_blueprint = new Blueprint();

	//Bind the custom function if it has been passed
	if (v_addObjFunc)
		v_blueprint->m_addObjectFunction = v_addObjFunc;

	v_blueprint->LoadBodies(v_root);
	v_blueprint->LoadJoints(v_root);

	return v_blueprint;
}

Blueprint* Blueprint::FromJsonString(const std::string& json_str)
{
	simdjson::dom::document v_bp_doc;
	if (!JsonReader::ParseSimdjsonString(json_str, v_bp_doc))
		return nullptr;

	const auto v_root = v_bp_doc.root();
	if (!v_root.is_object())
		return nullptr;

	Blueprint* nBlueprint = new Blueprint();

	nBlueprint->LoadBodies(v_root);
	nBlueprint->LoadJoints(v_root);

	return nBlueprint;
}

std::string Blueprint::GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const
{
	return "BLUEPRINT_MTL_NAME_NOT_NEEDED";
}

void Blueprint::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	for (const SMEntity* pEntity : this->Objects)
		pEntity->FillTextureMap(tex_map);
}

void Blueprint::WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const
{
	const glm::mat4 blueprint_matrix = transform_matrix * this->GetTransformMatrix();

	for (const SMEntity* pEntity : this->Objects)
		pEntity->WriteObjectToFile(file, mOffset, blueprint_matrix);
}

void Blueprint::AddObject_Default(Blueprint* self, SMEntity* v_entity)
{
	self->Objects.push_back(v_entity);
}

glm::vec3 Blueprint::JsonToVector(const simdjson::simdjson_result<simdjson::dom::element>& vec_json)
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

void Blueprint::LoadBodies(const simdjson::dom::element& pJson)
{
	const auto v_body_array = pJson["bodies"];
	if (!v_body_array.is_array()) return;

	for (const auto v_body : v_body_array.get_array())
	{
		const auto v_childs_array = v_body["childs"];
		if (!v_childs_array.is_array()) continue;

		for (const auto v_child : v_childs_array)
		{
			if (!v_child.is_object()) continue;

			const auto v_uuid = v_child["shapeId"];
			const auto v_position = v_child["pos"];
			const auto v_x_axis = v_child["xaxis"];
			const auto v_z_axis = v_child["zaxis"];
			const auto v_bounds = v_child["bounds"];
			const auto v_color = v_child["color"];

			if (!(v_uuid.is_string() && v_color.is_string())) continue;

			const int v_xAxisInt = (v_x_axis.is_number() ? JsonReader::GetNumber<int>(v_x_axis) : 1);
			const int v_zAxisInt = (v_z_axis.is_number() ? JsonReader::GetNumber<int>(v_z_axis) : 3);

			const glm::vec3 v_obj_pos = Blueprint::JsonToVector(v_position);
			const SMUuid v_obj_uuid = v_uuid.get_c_str();
			const SMColor v_obj_color = v_color.get_c_str();

			if (v_bounds.is_object())
			{
				const glm::vec3 v_blk_bounds = Blueprint::JsonToVector(v_bounds);

				if (!(v_blk_bounds.x > 0.0f && v_blk_bounds.y > 0.0f && v_blk_bounds.z > 0.0f))
					continue;

				BlockData* v_blk_data = Mod::GetGlobalBlock(v_obj_uuid);
				if (!v_blk_data) continue;

				Block* v_new_blk = new Block(v_blk_data, v_blk_bounds, v_obj_color, v_xAxisInt, v_zAxisInt);
				v_new_blk->SetPosition(v_obj_pos);
				v_new_blk->SetSize(v_blk_bounds);

				this->m_addObjectFunction(this, v_new_blk);
			}
			else
			{
				PartData* v_prt_data = Mod::GetGlobalPart(v_obj_uuid);
				if (!v_prt_data) continue;

				Model* v_prt_model = ModelStorage::LoadModel(v_prt_data->Mesh);
				if (!v_prt_model) continue;

				Part* v_new_prt = new Part(v_prt_data, v_prt_model, v_obj_color, v_xAxisInt, v_zAxisInt);
				v_new_prt->SetPosition(v_obj_pos);

				this->m_addObjectFunction(this, v_new_prt);
			}
		}
	}
}

void Blueprint::LoadJoints(const simdjson::dom::element& pJson)
{
	const auto v_jnt_array = pJson["joints"];
	if (!v_jnt_array.is_array())
		return;

	for (const auto v_jnt : v_jnt_array)
	{
		const auto v_x_axis = v_jnt["xaxisA"];
		const auto v_z_axis = v_jnt["zaxisA"];
		const auto v_uuid = v_jnt["shapeId"];
		const auto v_color = v_jnt["color"];
		//const auto& jChildA = JsonReader::Get(pJoint, "childA"); might implement later
		const auto v_position = v_jnt["posA"];

		if (!(v_uuid.is_string() && v_color.is_string())) continue;

		const int v_xAxisInt = (v_x_axis.is_number() ? JsonReader::GetNumber<int>(v_x_axis) : 1);
		const int v_zAxisInt = (v_z_axis.is_number() ? JsonReader::GetNumber<int>(v_z_axis) : 3);

		const glm::vec3 v_pos_vec = Blueprint::JsonToVector(v_position);

		const SMUuid v_jnt_uuid = v_uuid.get_c_str();
		const SMColor v_jnt_color = v_color.get_c_str();

		PartData* v_jnt_data = Mod::GetGlobalPart(v_jnt_uuid);
		if (!v_jnt_data) continue;

		Model* v_jnt_model = ModelStorage::LoadModel(v_jnt_data->Mesh);
		if (!v_jnt_model) continue;

		Joint* v_new_jnt = new Joint(v_jnt_data, v_jnt_model, v_jnt_color, v_xAxisInt, v_zAxisInt);
		v_new_jnt->SetPosition(v_pos_vec);

		this->m_addObjectFunction(this, v_new_jnt);
	}
}
#pragma once

#include "Converter/Entity/Entity.hpp"
#include "Converter/Entity/Block.hpp"
#include "Converter/Entity/Joint.hpp"
#include "Converter/Entity/Part.hpp"
#include "Converter/ConvertError.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Json.hpp"

SM_UNMANAGED_CODE

class SMBlueprint : public SMEntity
{
	friend class BlueprintConv;

	inline SMBlueprint(const glm::vec3& pos, const glm::quat& rot)
		: SMEntity(pos, rot, glm::vec3(0.25f)), m_object_index(0), m_body_index(0) {}

public:
	using AddObjectFunction = void (*)(SMBlueprint*, SMEntity*);

	//For object counter
	static void LoadAndCountAutomatic(const std::string& str);
	static void CountFromFile(const std::wstring& path);
	static void CountFromJsonString(const std::string_view& str);

	//For object loader
	static SMBlueprint* LoadAutomatic(const std::string& str, const glm::vec3& pos, const glm::quat& rot);
	static SMBlueprint* FromFile(const std::wstring& path, const glm::vec3& pos, const glm::quat& rot);
	//Used by blueprint converter as it reports the conversion status
	static SMBlueprint* FromFileWithStatus(const std::wstring& path, AddObjectFunction v_addObjFunc, ConvertError& v_error);
	static SMBlueprint* FromJsonString(const std::string_view& json_str, const glm::vec3& pos, const glm::quat& rot);

	inline EntityType Type() const noexcept override { return EntityType::Blueprint; }
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	void WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const override;
	std::size_t GetAmountOfObjects() const override;
	void CalculateCenterPoint(glm::vec3& v_input) const override;

	inline ~SMBlueprint()
	{
		for (SMEntity* pObject : this->Objects)
			delete pObject;
	}

	/*
		This vector contains blocks, parts and joints
		When in blueprint converter mode, this vector only stores bodies
	*/
	std::vector<SMEntity*> Objects = {};

private:
	AddObjectFunction m_addObjectFunction = SMBlueprint::AddObject_Default;

	static void AddObject_Default(SMBlueprint* self, SMEntity* v_entity);

	static glm::vec3 JsonToVector(const simdjson::simdjson_result<simdjson::dom::element>& vec_json);

	void LoadChild(const simdjson::dom::element& v_child);
	void LoadJoint(const simdjson::dom::element& v_jnt);

	void LoadBodiesWithCounter(const simdjson::dom::element& v_blueprint);
	void LoadJointsWithCounter(const simdjson::dom::element& v_blueprint);

	void LoadBodies(const simdjson::dom::element& pJson);
	void LoadJoints(const simdjson::dom::element& pJson);

	std::size_t m_object_index;
	std::size_t m_body_index;
};

SM_MANAGED_CODE
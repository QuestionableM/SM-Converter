#pragma once

#include "Converter\Entity\Entity.hpp"
#include "Converter\Entity\Block.hpp"
#include "Converter\Entity\Joint.hpp"
#include "Converter\Entity\Part.hpp"
#include "Converter\ConvertError.hpp"

#include "Utils\Json.hpp"

class Blueprint : public SMEntity
{
	inline Blueprint()
	{
		this->m_size = glm::vec3(0.25f);
	}

public:
	static Blueprint* LoadAutomatic(const std::string& str);
	static Blueprint* FromFile(const std::wstring& path);
	static Blueprint* FromFileErrorChecked(const std::wstring& path, void (*v_addObjFunc)(Blueprint*, SMEntity*), ConvertError& v_error);
	static Blueprint* FromJsonString(const std::string& json_str);

	inline EntityType Type() const override { return EntityType::Blueprint; }
	std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const override;
	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override;
	void WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const override;
	inline std::size_t GetAmountOfObjects() const override { return this->Objects.size(); }

	inline ~Blueprint()
	{
		for (SMEntity*& pObject : this->Objects)
			delete pObject;
	}

	/*
		This vector contains blocks, parts and joints
		When in blueprint converter mode, this vector only stores bodies
	*/
	std::vector<SMEntity*> Objects = {};

private:
	void (*m_addObjectFunction)(Blueprint*, SMEntity*) = Blueprint::AddObject_Default;

	static void AddObject_Default(Blueprint* self, SMEntity* v_entity);

	static glm::vec3 JsonToVector(const simdjson::simdjson_result<simdjson::dom::element>& vec_json);

	void LoadBodies(const simdjson::dom::element& pJson);
	void LoadJoints(const simdjson::dom::element& pJson);
};
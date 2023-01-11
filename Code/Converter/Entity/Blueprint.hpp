#pragma once

#include "Converter\Entity\Entity.hpp"
#include "Converter\Entity\Block.hpp"
#include "Converter\Entity\Joint.hpp"
#include "Converter\Entity\Part.hpp"

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
	static Blueprint* FromJsonString(const std::string& json_str);

	//This vector contains blocks, parts and joints
	std::vector<SMEntity*> Objects = {};

	inline void AddObject(SMEntity* object)
	{
		//Check if the object is valid (Valid objects: Block, Part, Joint)
		assert((static_cast<unsigned short>(object->Type()) & 0b11100000) != 0);

		this->Objects.push_back(object);
	}

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

private:
	static glm::vec3 JsonToVector(const simdjson::simdjson_result<simdjson::dom::element>& vec_json);

	void LoadBodies(const simdjson::dom::element& pJson);
	void LoadJoints(const simdjson::dom::element& pJson);
};
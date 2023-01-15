#pragma once

#include "ObjectDatabase\ObjectData.hpp"
#include "Converter\WriterOffset.hpp"

#include "Utils\Color.hpp"
#include "Utils\Uuid.hpp"

#include <string>

#include <gtx\quaternion.hpp>
#include <gtx\transform.hpp>
#include <glm.hpp>

struct ObjectTexData
{
	TextureList Textures;
	SMColor TexColor;
};

struct Model;

enum class EntityType : unsigned short
{
	Harvestable = (1 << 0),
	Blueprint   = (1 << 1),
	Clutter     = (1 << 2),
	Prefab      = (1 << 3),
	Asset       = (1 << 4),
	Block       = (1 << 5),
	Joint       = (1 << 6),
	Part        = (1 << 7),
	Decal       = (1 << 8),
	Body        = (1 << 9),

	//Is not used anywhere except the GroundTerrainData class
	GroundTerrain = 0
};

class SMEntity
{
public:
	virtual ~SMEntity() = default;

	inline virtual glm::mat4 GetTransformMatrix() const
	{
		glm::mat4 transform(1.0f);

		transform *= glm::translate(this->m_position);
		transform *= glm::toMat4(this->m_rotation);
		transform *= glm::scale(this->m_size);

		return transform;
	}

	inline glm::vec3 GetPosition() const { return this->m_position; }
	inline glm::quat GetRotation() const { return this->m_rotation; }
	inline glm::vec3 GetSize() const { return this->m_size; }
	inline SMUuid GetUuid() const { return this->m_uuid; }
	inline Model* GetModel() const { return this->m_model; }

	inline void SetPosition(const glm::vec3& pos) { this->m_position = pos; }
	inline void SetRotation(const glm::quat& rot) { this->m_rotation = rot; }
	inline void SetSize(const glm::vec3& size) { this->m_size = size; }
	inline void SetUuid(const SMUuid& uuid) { this->m_uuid = uuid; }
	inline void SetModel(Model* model) { this->m_model = model; }

	virtual EntityType Type() const = 0;
	virtual std::size_t GetIndex() const { return -1; }
	virtual std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const = 0;
	virtual void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const = 0;
	virtual void WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const;
	inline virtual std::size_t GetAmountOfObjects() const { return 1; }
	inline virtual SMColor GetColor() const { return static_cast<int>(0x000000ff); }

protected:
	glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat m_rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 m_size = glm::vec3(1.0f, 1.0f, 1.0f);

	SMUuid m_uuid;
	Model* m_model;
};
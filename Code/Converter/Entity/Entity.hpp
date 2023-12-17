#pragma once

#include "ObjectDatabase\ObjectData.hpp"
#include "Converter\WriterOffset.hpp"

#include "UStd\UnmanagedString.hpp"

#include "Utils\GlmUnmanaged.hpp"
#include "Utils\Color.hpp"
#include "Utils\Uuid.hpp"

#pragma unmanaged

struct ObjectTexData
{
	SMTextureList m_textures;
	SMColor m_tex_color;
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
	Kinematic   = (1 << 10),

	//Is not used anywhere except the GroundTerrainData class
	GroundTerrain = 0
};

struct SMEntityTransform
{
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
};

class SMEntity
{
public:
	virtual ~SMEntity() = default;

	SMEntity(const SMEntity&) = delete;
	SMEntity(SMEntity&&) = delete;

	SMEntity()
		: m_position(0.0f), m_rotation(), m_size(1.0f) {}

	SMEntity(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale)
		: m_position(pos), m_rotation(rot), m_size(scale) {}

	SMEntity(const SMEntityTransform& transform)
		: m_position(transform.position), m_rotation(transform.rotation), m_size(transform.scale) {}

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

	inline void SetPosition(const glm::vec3& pos) { this->m_position = pos; }
	inline void SetRotation(const glm::quat& rot) { this->m_rotation = rot; }
	inline void SetSize(const glm::vec3& size) { this->m_size = size; }

	virtual const SMUuid& GetUuid() const { return SMUuid::Null; }

	virtual EntityType Type() const noexcept = 0;
	virtual std::size_t GetIndex() const { return -1; }
	virtual char* GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const { return v_ptr; }
	virtual std::string GetMtlName(std::size_t v_idx) const { return ""; }
	virtual void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const = 0;
	virtual void WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const {}
	inline virtual void CalculateCenterPoint(glm::vec3& v_input) const { v_input += m_position; }
	inline virtual std::size_t GetAmountOfObjects() const { return 1; }
	inline virtual SMColor GetColor() const { return static_cast<int>(0x000000ff); }
	//This option can help you to discard unwanted sub meshes (like shadow only sub meshes)
	inline virtual bool GetCanWrite(const std::string& name, std::size_t v_idx) const { return true; }

protected:
	glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat m_rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 m_size = glm::vec3(1.0f, 1.0f, 1.0f);
};

class SMEntityWithUuid : public SMEntity
{
public:
	using SMEntity::SMEntity;

	virtual ~SMEntityWithUuid() = default;

	SMEntityWithUuid(const SMUuid& uuid)
		: m_uuid(uuid) {}

	SMEntityWithUuid(const SMUuid& uuid, const glm::vec3& pos, const glm::vec3& scale)
		: SMEntity(pos, glm::quat(), scale), m_uuid(uuid) {}
	
	SMEntityWithUuid(const SMUuid& uuid, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale)
		: SMEntity(pos, rot, scale), m_uuid(uuid) {}

	SMEntityWithUuid(const SMUuid& uuid, const SMEntityTransform& transform)
		: SMEntity(transform), m_uuid(uuid) {}

	virtual const SMUuid& GetUuid() const override { return m_uuid; }

protected:
	SMUuid m_uuid;
};

class SMEntityWithModel : public SMEntity
{
public:
	virtual ~SMEntityWithModel() = default;

	SMEntityWithModel(Model* pModel)
		: m_model(pModel) {}

	SMEntityWithModel(Model* pModel, const glm::vec3& pos)
		: SMEntity(pos, glm::quat(), glm::vec3(1.0f)), m_model(pModel) {}

	SMEntityWithModel(Model* pModel, const SMEntityTransform& transform)
		: SMEntity(transform), m_model(pModel) {}

	virtual void WriteObjectToFile(
		std::ofstream& file,
		WriterOffsetData& mOffset,
		const glm::mat4& transform_matrix) const override;

protected:
	Model* m_model;
};

class SMEntityWithModelAndUuid : public SMEntityWithModel
{
public:
	virtual ~SMEntityWithModelAndUuid() = default;

	SMEntityWithModelAndUuid(const SMUuid& uuid, Model* pModel)
		: SMEntityWithModel(pModel), m_uuid(uuid) {}

	SMEntityWithModelAndUuid(const SMUuid& uuid, Model* pModel, const glm::vec3& pos)
		: SMEntityWithModel(pModel, pos), m_uuid(uuid) {}

	SMEntityWithModelAndUuid(const SMUuid& uuid, Model* pModel, const SMEntityTransform& transform)
		: SMEntityWithModel(pModel, transform), m_uuid(uuid) {}

	virtual const SMUuid& GetUuid() const override { return m_uuid; }

protected:
	SMUuid m_uuid;
};

#pragma managed
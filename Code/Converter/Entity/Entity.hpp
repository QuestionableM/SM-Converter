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
	virtual std::string GetMtlName(const std::string& mat_name, const std::size_t& mIdx) const { return ""; }
	virtual void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const = 0;
	virtual void WriteObjectToFile(std::ofstream& file, WriterOffsetData& mOffset, const glm::mat4& transform_matrix) const;
	inline virtual void CalculateCenterPoint(glm::vec3& v_input) const { v_input += m_position; }
	inline virtual std::size_t GetAmountOfObjects() const { return 1; }
	inline virtual SMColor GetColor() const { return static_cast<int>(0x000000ff); }
	//This option can help you to discard unwanted sub meshes (like shadow only sub meshes)
	inline virtual bool GetCanWrite(const std::string& name, const std::size_t& v_idx) const { return true; }

protected:
	glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat m_rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 m_size = glm::vec3(1.0f, 1.0f, 1.0f);

	SMUuid m_uuid;
	Model* m_model;
};

#pragma managed
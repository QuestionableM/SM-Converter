#pragma once

#include "ObjectDatabase/ObjectData.hpp"
#include "Converter/WriterOffset.hpp"

#include "UStd/UnmanagedString.hpp"

#include "Utils/GlmUnmanaged.hpp"
#include "Utils/clr_include.hpp"
#include "Utils/Color.hpp"
#include "Utils/Uuid.hpp"

SM_UNMANAGED_CODE

struct Model;

// A fast in place constructor for unordered_maps
struct ObjectTexDataConstructInfo
{
	inline ObjectTexDataConstructInfo(const SMTextureList& texList, const SMColor color)
		: m_texturesRef(texList)
		, m_texColor(color)
	{}

	const SMTextureList& m_texturesRef;
	SMColor m_texColor;
};

struct ObjectTexData
{
	ObjectTexData() = default;
	ObjectTexData(const SMColor color);
	ObjectTexData(const SMTextureList& texList, const SMColor color);
	ObjectTexData(const ObjectTexDataConstructInfo& info);

	ObjectTexData(const ObjectTexData& other) = default;
	ObjectTexData(ObjectTexData&& other) noexcept;

	void operator=(ObjectTexData&& other) noexcept;

	SMTextureList m_textures;
	SMColor m_texColor;
};

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
	Wedge       = (1 << 11),

	//Is not used anywhere except the GroundTerrainData class
	GroundTerrain = 0
};

struct SMEntityTransform
{
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
};

class SMC_NOVTABLE SMEntity
{
public:
	using EntityTextureMap = std::unordered_map<std::string, ObjectTexData, Hashing::StringHasher, std::equal_to<>>;

	SMEntity();
	SMEntity(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale);
	SMEntity(const SMEntityTransform& transform);
	virtual ~SMEntity() = default;

	SMEntity(const SMEntity&) = delete;
	SMEntity(SMEntity&&) = delete;

	glm::vec3 GetPosition() const noexcept;
	glm::quat GetRotation() const noexcept;
	glm::vec3 GetSize() const noexcept;

	void SetPosition(const glm::vec3& pos) noexcept;
	void SetRotation(const glm::quat& rot) noexcept;
	void SetSize(const glm::vec3& size) noexcept;

	virtual glm::mat4 GetTransformMatrix() const;
	virtual const SMUuid& GetUuid() const;

	virtual EntityType Type() const = 0;
	virtual std::size_t GetIndex() const;
	virtual char* GetMtlNameCStr(const std::string_view& material, const std::size_t idx, char* pCString) const;
	virtual std::string GetMtlName(const std::size_t idx) const;
	virtual void FillTextureMap(EntityTextureMap& textureMap) const = 0;
	virtual void WriteObjectToFile(std::ofstream& file, WriterOffsetData& offset, const glm::mat4& transform) const;
	virtual void CalculateCenterPoint(glm::vec3& outInput) const;
	virtual std::size_t GetAmountOfObjects() const;
	virtual SMColor GetColor() const;
	// This option can help you to discard unwanted sub meshes (like shadow only sub meshes)
	virtual bool GetCanWrite(const std::string_view& name, const std::size_t idx) const;

protected:
	glm::vec3 m_position;
	glm::quat m_rotation;
	glm::vec3 m_size;
};

class SMC_NOVTABLE SMEntityWithUuid : public SMEntity
{
public:
	using SMEntity::SMEntity;

	SMEntityWithUuid(const SMUuid& uuid);
	SMEntityWithUuid(const SMUuid& uuid, const glm::vec3& pos, const glm::vec3& scale);
	SMEntityWithUuid(const SMUuid& uuid, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale);
	SMEntityWithUuid(const SMUuid& uuid, const SMEntityTransform& transform);
	virtual ~SMEntityWithUuid() = default;

	const SMUuid& GetUuid() const final override;

protected:
	SMUuid m_uuid;
};

class SMC_NOVTABLE SMEntityWithModel : public SMEntity
{
public:
	SMEntityWithModel(Model* pModel);
	SMEntityWithModel(Model* pModel, const glm::vec3& pos);
	SMEntityWithModel(Model* pModel, const SMEntityTransform& transform);
	virtual ~SMEntityWithModel() = default;

	void WriteObjectToFile(
		std::ofstream& file,
		WriterOffsetData& offset,
		const glm::mat4& transform) const override;

protected:
	Model* m_model;
};

class SMC_NOVTABLE SMEntityWithModelAndUuid : public SMEntityWithModel
{
public:
	SMEntityWithModelAndUuid(const SMUuid& uuid, Model* pModel);
	SMEntityWithModelAndUuid(const SMUuid& uuid, Model* pModel, const glm::vec3& pos);
	SMEntityWithModelAndUuid(const SMUuid& uuid, Model* pModel, const SMEntityTransform& transform);
	virtual ~SMEntityWithModelAndUuid() = default;

	const SMUuid& GetUuid() const override;

protected:
	SMUuid m_uuid;
};

SM_MANAGED_CODE
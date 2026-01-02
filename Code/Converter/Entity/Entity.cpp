#include "Entity.hpp"

#include "ObjectDatabase/ModelStorage.hpp"
#include "ObjectDatabase/ProgCounter.hpp"

SM_UNMANAGED_CODE

ObjectTexData::ObjectTexData(const SMColor color)
	: m_textures()
	, m_texColor(color)
{}

ObjectTexData::ObjectTexData(const SMTextureList& texList, const SMColor color)
	: m_textures(texList)
	, m_texColor(color)
{}

ObjectTexData::ObjectTexData(const ObjectTexDataConstructInfo& info)
	: m_textures(info.m_texturesRef)
	, m_texColor(info.m_texColor)
{}

ObjectTexData::ObjectTexData(ObjectTexData&& other) noexcept
	: m_textures(std::move(other.m_textures))
	, m_texColor(other.m_texColor)
{}

void ObjectTexData::operator=(ObjectTexData&& other) noexcept
{
	m_textures = std::move(other.m_textures);
	m_texColor = other.m_texColor;
}

//
// SM ENTITY
//

SMEntity::SMEntity()
	: m_position(0.0f)
	, m_rotation()
	, m_size(1.0f)
{}

SMEntity::SMEntity(const SMEntityTransform& transform)
	: m_position(transform.position)
	, m_rotation(transform.rotation)
	, m_size(transform.scale)
{}

SMEntity::SMEntity(
	const glm::vec3& pos,
	const glm::quat& rot,
	const glm::vec3& scale
)
	: m_position(pos)
	, m_rotation(rot)
	, m_size(scale)
{}

glm::vec3 SMEntity::GetPosition() const noexcept
{
	return m_position;
}

glm::quat SMEntity::GetRotation() const noexcept
{
	return m_rotation;
}

glm::vec3 SMEntity::GetSize() const noexcept
{
	return m_size;
}

void SMEntity::SetPosition(const glm::vec3& pos) noexcept
{
	m_position = pos;
}

void SMEntity::SetRotation(const glm::quat& rot) noexcept
{
	m_rotation = rot;
}

void SMEntity::SetSize(const glm::vec3& size) noexcept
{
	m_size = size;
}

glm::mat4 SMEntity::GetTransformMatrix() const
{
	glm::mat4 v_transform(1.0f);

	v_transform *= glm::translate(m_position);
	v_transform *= glm::toMat4(m_rotation);
	v_transform *= glm::scale(m_size);

	return v_transform;
}

const SMUuid& SMEntity::GetUuid() const
{
	return SMUuid::Null;
}

std::size_t SMEntity::GetIndex() const
{
	return std::size_t(-1);
}

char* SMEntity::GetMtlNameCStr(
	const std::string_view& material,
	const std::size_t idx,
	char* pCString) const
{
	return pCString;
}

std::string SMEntity::GetMtlName(const std::size_t idx) const
{
	return "";
}

void SMEntity::WriteObjectToFile(
	std::ofstream& file,
	WriterOffsetData& offset,
	const glm::mat4& transform) const
{}

void SMEntity::CalculateCenterPoint(glm::vec3& outInput) const
{
	outInput += m_position;
}

std::size_t SMEntity::GetAmountOfObjects() const
{
	return 1;
}

SMColor SMEntity::GetColor() const
{
	return static_cast<std::uint32_t>(0x000000ff);
}

bool SMEntity::GetCanWrite(
	const std::string_view& name,
	const std::size_t idx) const
{
	return true;
}

//
// SM ENTITY WITH UUID
//

SMEntityWithUuid::SMEntityWithUuid(const SMUuid& uuid)
	: m_uuid(uuid)
{}


SMEntityWithUuid::SMEntityWithUuid(
	const SMUuid& uuid,
	const glm::vec3& pos,
	const glm::vec3& scale
)
	: SMEntity(pos, glm::quat(), scale)
	, m_uuid(uuid)
{}

SMEntityWithUuid::SMEntityWithUuid(
	const SMUuid& uuid,
	const glm::vec3& pos,
	const glm::quat& rot,
	const glm::vec3& scale
)
	: SMEntity(pos, rot, scale)
	, m_uuid(uuid)
{}

SMEntityWithUuid::SMEntityWithUuid(
	const SMUuid& uuid,
	const SMEntityTransform& transform
)
	: SMEntity(transform)
	, m_uuid(uuid)
{}

const SMUuid& SMEntityWithUuid::GetUuid() const
{
	return m_uuid;
}

//
// SM ENTITY WITH MODEL
//

SMEntityWithModel::SMEntityWithModel(Model* pModel)
	: m_model(pModel)
{}

SMEntityWithModel::SMEntityWithModel(
	Model* pModel,
	const glm::vec3& pos
)
	: SMEntity(pos, glm::quat(), glm::vec3(1.0f))
	, m_model(pModel)
{}

SMEntityWithModel::SMEntityWithModel(
	Model* pModel,
	const SMEntityTransform& transform
)
	: SMEntity(transform)
	, m_model(pModel)
{}

void SMEntityWithModel::WriteObjectToFile(
	std::ofstream& file,
	WriterOffsetData& offset,
	const glm::mat4& transform) const
{
	const glm::mat4 model_matrix = transform * this->GetTransformMatrix();

	m_model->WriteToFile(model_matrix, offset, file, this);

	ProgCounter::ProgressValue++;
}

//
// SM ENTITY WITH MODEL AND UUID
//

SMEntityWithModelAndUuid::SMEntityWithModelAndUuid(
	const SMUuid& uuid,
	Model* pModel
)
	: SMEntityWithModel(pModel)
	, m_uuid(uuid)
{}

SMEntityWithModelAndUuid::SMEntityWithModelAndUuid(
	const SMUuid& uuid,
	Model* pModel,
	const glm::vec3& pos
)
	: SMEntityWithModel(pModel, pos)
	, m_uuid(uuid)
{}

SMEntityWithModelAndUuid::SMEntityWithModelAndUuid(
	const SMUuid& uuid,
	Model* pModel,
	const SMEntityTransform& transform
)
	: SMEntityWithModel(pModel, transform)
	, m_uuid(uuid)
{}

const SMUuid& SMEntityWithModelAndUuid::GetUuid() const
{
	return m_uuid;
}
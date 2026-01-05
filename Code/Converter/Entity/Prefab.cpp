#include "Prefab.hpp"

#include "ObjectDatabase/ModelStorage.hpp"

SM_UNMANAGED_CODE

SMPrefab::SMPrefab(
	const std::wstring_view& path,
	const std::wstring_view& flag,
	const SMEntityTransform& transform
)
	: SMEntity(transform)
	, m_path(path)
	, m_flag(flag)
	, m_objects()
{}

SMPrefab::~SMPrefab()
{
	for (SMEntity* v_pEntity : m_objects)
		delete v_pEntity;
}

const std::wstring& SMPrefab::getPath() const noexcept
{
	return m_path;
}

const std::wstring& SMPrefab::getFlag() const noexcept
{
	return m_flag;
}

const std::vector<SMEntity*>& SMPrefab::getObjects() const noexcept
{
	return m_objects;
}

void SMPrefab::addObject(SMEntity* object)
{
	//Check if the object is valid (Valid objects: Harvestable, Blueprint, Prefab, Asset, Decal, Kinematic)
	assert((static_cast<unsigned short>(object->Type()) & 0b10100011011) != 0);

	m_objects.push_back(object);
}

EntityType SMPrefab::Type() const
{
	return EntityType::Prefab;
}

void SMPrefab::WriteObjectToFile(
	std::ofstream& file,
	WriterOffsetData& offset,
	const glm::mat4& transform) const
{
	const glm::mat4 v_prefabMatrix = transform * this->GetTransformMatrix();

	for (const SMEntity* pEntity : m_objects)
		pEntity->WriteObjectToFile(file, offset, v_prefabMatrix);
}

void SMPrefab::FillTextureMap(EntityTextureMap& textureMap) const
{
	for (const SMEntity* pEntity : m_objects)
		pEntity->FillTextureMap(textureMap);
}

std::size_t SMPrefab::GetAmountOfObjects() const
{
	std::size_t output = 0;

	for (const SMEntity* tEntity : m_objects)
		output += tEntity->GetAmountOfObjects();

	return output;
}
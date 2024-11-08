#include "Prefab.hpp"

#include "ObjectDatabase\ModelStorage.hpp"

#pragma unmanaged

SMPrefab::SMPrefab(
	const std::wstring& path,
	const std::wstring& flag,
	const SMEntityTransform& transform
) :
	SMEntity(transform),
	m_path(path),
	m_flag(flag),
	m_objects()
{}

SMPrefab::~SMPrefab()
{
	for (SMEntity* v_pEntity : m_objects)
		delete v_pEntity;
}

const std::wstring& SMPrefab::getPath() const
{
	return m_path;
}

const std::wstring& SMPrefab::getFlag() const
{
	return m_flag;
}

const std::vector<SMEntity*>& SMPrefab::getObjects() const
{
	return m_objects;
}

void SMPrefab::addObject(SMEntity* object)
{
	//Check if the object is valid (Valid objects: Harvestable, Blueprint, Prefab, Asset, Decal, Kinematic)
	assert((static_cast<unsigned short>(object->Type()) & 0b10100011011) != 0);

	m_objects.push_back(object);
}

void SMPrefab::WriteObjectToFile(
	std::ofstream& file,
	WriterOffsetData& mOffset,
	const glm::mat4& transform_matrix) const
{
	const glm::mat4 v_prefabMatrix = transform_matrix * this->GetTransformMatrix();

	for (const SMEntity* pEntity : m_objects)
		pEntity->WriteObjectToFile(file, mOffset, v_prefabMatrix);
}

void SMPrefab::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	for (const SMEntity* pEntity : m_objects)
		pEntity->FillTextureMap(tex_map);
}

std::size_t SMPrefab::GetAmountOfObjects() const
{
	std::size_t output = 0;

	for (const SMEntity* tEntity : m_objects)
		output += tEntity->GetAmountOfObjects();

	return output;
}
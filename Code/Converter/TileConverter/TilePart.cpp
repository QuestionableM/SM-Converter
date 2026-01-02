#include "TilePart.hpp"

#include "Converter/TileConverter/Tile.hpp"

#include "Utils/GlmUnmanaged.hpp"
#include "Utils/Console.hpp"
#include "Utils/String.hpp"

SM_UNMANAGED_CODE

TilePart::TilePart()
	: m_vertexHeight()
	, m_vertexColor()
	, m_ground()
	, m_clutter()
	, m_clutterMap()
	, m_objects()
{}

TilePart::~TilePart()
{
	for (SMTileClutter* v_pCurClutter : m_clutterMap)
		delete v_pCurClutter;

	for (std::vector<SMEntity*>& v_curEntitiesVec : m_objects)
		for (SMEntity* v_pCurEntity : v_curEntitiesVec)
			delete v_pCurEntity;
}

void TilePart::AddObject(SMEntity* object, int index)
{
	assert(object != nullptr);
	assert(0 <= index && index <= 3);
	assert((static_cast<unsigned short>(object->Type()) & 0b10100011011) != 0);
	assert((index > 0 && (static_cast<unsigned short>(object->Type()) & 0b00010001) != 0) || index == 0);

	this->m_objects[index].push_back(object);
}

void TilePart::WriteToFile(
	std::ofstream& model,
	WriterOffsetData& offset,
	int tileWidth,
	int tileHeight,
	int xPos,
	int zPos)
{
	constexpr const float v_fRotOffset = 1.0f * glm::pi<float>();

	constexpr const float v_fTileSize = 64.0f;
	const float tWidth  = float(tileWidth ) * v_fTileSize;
	const float tHeight = float(tileHeight) * v_fTileSize;

	const glm::vec3 v_halfPoint(tWidth / 2.0f, tHeight / 2.0f, 0.0f);

	glm::mat4 v_transform(1.0f);
	v_transform *= glm::translate(-v_halfPoint);
	v_transform *= glm::rotate(v_fRotOffset, glm::vec3(0.0f, 0.0f, 1.0f));
	v_transform *= glm::translate(v_halfPoint);
	v_transform *= glm::translate(glm::vec3((float)xPos * 64.0f, (float)zPos * 64.0f, 0.0f));
	v_transform *= glm::translate(glm::vec3(-tWidth * 1.5f, -tHeight * 1.5f, 0.0f));

	for (const std::vector<SMEntity*>& v_curEntitiesVec : m_objects)
		for (const SMEntity* v_pCurEntity : v_curEntitiesVec)
			v_pCurEntity->WriteObjectToFile(model, offset, v_transform);
}

void TilePart::WriteToFileWorld(
	std::ofstream& model,
	WriterOffsetData& offset,
	std::size_t posX,
	std::size_t posY,
	std::size_t worldSz,
	char rotation)
{
	float v_rotOffset = 0.0f;
	switch (rotation)
	{
	case 1: v_rotOffset = glm::half_pi<float>(); break;
	case 2: v_rotOffset = glm::pi<float>(); break;
	case 3: v_rotOffset = 3.0f * glm::half_pi<float>(); break;
	}

	constexpr const float v_tileSz = 64.0f;
	const float v_wldSize = static_cast<float>(worldSz) * v_tileSz;
	const float v_wldSizeHalf = v_wldSize / 2.0f;

	const glm::vec3 v_tileHalf(v_tileSz / 2.0f, v_tileSz / 2.0f, 0.0f);

	glm::mat4 v_transform(1.0f);
	v_transform *= glm::translate(glm::vec3(-v_wldSizeHalf, -v_wldSizeHalf, 0.0f));
	v_transform *= glm::translate(glm::vec3(static_cast<float>(posX) * v_tileSz, static_cast<float>(posY) * v_tileSz, 0.0f));
	v_transform *= glm::translate(v_tileHalf);
	v_transform *= glm::rotate(v_rotOffset, glm::vec3(0.0f, 0.0f, 1.0f));
	v_transform *= glm::translate(-v_tileHalf);

	for (const std::vector<SMEntity*>& v_curEntitiesVec : m_objects)
		for (const SMEntity* v_pCurEntity : v_curEntitiesVec)
			v_pCurEntity->WriteObjectToFile(model, offset, v_transform);
}

void TilePart::FillTextureMap(SMEntity::EntityTextureMap& textureMap) const
{
	for (const std::vector<SMEntity*>& v_curEntitiesVec : m_objects)
		for (const SMEntity* v_pCurEntity : v_curEntitiesVec)
			v_pCurEntity->FillTextureMap(textureMap);

	for (const SMTileClutter* v_pCurClutter : m_clutterMap)
	{
		if (v_pCurClutter)
			v_pCurClutter->FillTextureMap(textureMap);
	}
}

std::size_t TilePart::GetAmountOfObjects() const
{
	std::size_t v_output = 0;

	for (const auto& v_vecEntities : m_objects)
		for (const SMEntity* v_pCurEntity : v_vecEntities)
			v_output += v_pCurEntity->GetAmountOfObjects();

	return v_output;
}
#include "TilePart.hpp"

#include "Converter\TileConverter\Tile.hpp"

#include "Utils\GlmUnmanaged.hpp"
#include "Utils\Console.hpp"
#include "Utils\String.hpp"

#pragma unmanaged

TilePart::TilePart(Tile* parent)
{
	this->Parent = parent;

	//set all pointers to zero
	std::memset(m_ClutterMap.data(), 0, (128 * 128) * sizeof(std::size_t));
}

TilePart::~TilePart()
{
	for (std::size_t a = 0; a < m_ClutterMap.size(); a++)
		delete m_ClutterMap[a];

	for (std::size_t a = 0; a < m_Objects.size(); a++)
	{
		for (std::size_t b = 0; b < m_Objects[a].size(); b++)
			delete m_Objects[a][b];
	}
}

void TilePart::AddObject(SMEntity* object, int index)
{
	assert(object != nullptr);
	assert(0 <= index && index <= 3);
	assert((static_cast<unsigned short>(object->Type()) & 0b10100011011) != 0);
	assert((index > 0 && (static_cast<unsigned short>(object->Type()) & 0b00010001) != 0) || index == 0);

	this->m_Objects[index].push_back(object);
}

void TilePart::WriteToFile(std::ofstream& model, WriterOffsetData& mOffsetData, int xPos, int zPos)
{
	constexpr const float rot_offset = 1.0f * glm::pi<float>();

	constexpr const float tile_size = 64.0f;
	const float tWidth  = this->Parent->GetWidth()  * tile_size;
	const float tHeight = this->Parent->GetHeight() * tile_size;

	const glm::vec3 half_point(tWidth / 2.0f, tHeight / 2.0f, 0.0f);

	glm::mat4 transform(1.0f);
	transform *= glm::translate(-half_point);
	transform *= glm::rotate(rot_offset, glm::vec3(0.0f, 0.0f, 1.0f));
	transform *= glm::translate(half_point);
	transform *= glm::translate(glm::vec3((float)xPos * 64.0f, (float)zPos * 64.0f, 0.0f));
	transform *= glm::translate(glm::vec3(-tWidth * 1.5f, -tHeight * 1.5f, 0.0f));

	for (std::size_t vec_idx = 0; vec_idx < m_Objects.size(); vec_idx++)
	{
		const std::vector<SMEntity*>& entity_array = m_Objects[vec_idx];

		for (std::size_t a = 0; a < entity_array.size(); a++)
			entity_array[a]->WriteObjectToFile(model, mOffsetData, transform);
	}
}

void TilePart::WriteToFileWorld(std::ofstream& v_model,
	WriterOffsetData& v_offset,
	std::size_t x_pos,
	std::size_t y_pos,
	std::size_t v_world_sz,
	char v_rotation)
{
	float v_rot_offset = 0.0f;
	switch (v_rotation)
	{
	case 1: v_rot_offset = glm::half_pi<float>(); break;
	case 2: v_rot_offset = glm::pi<float>(); break;
	case 3: v_rot_offset = 3.0f * glm::half_pi<float>(); break;
	}

	constexpr const float v_tile_sz = 64.0f;
	const float v_world_size = static_cast<float>(v_world_sz) * v_tile_sz;
	const float v_world_size_half = v_world_size / 2.0f;

	const glm::vec3 v_tile_half(v_tile_sz / 2.0f, v_tile_sz / 2.0f, 0.0f);

	glm::mat4 transform(1.0f);
	transform *= glm::translate(glm::vec3(-v_world_size_half, -v_world_size_half, 0.0f));
	transform *= glm::translate(glm::vec3(static_cast<float>(x_pos) * v_tile_sz, static_cast<float>(y_pos) * v_tile_sz, 0.0f));
	transform *= glm::translate(v_tile_half);
	transform *= glm::rotate(v_rot_offset, glm::vec3(0.0f, 0.0f, 1.0f));
	transform *= glm::translate(-v_tile_half);

	for (std::size_t a = 0; a < m_Objects.size(); a++)
	{
		const std::vector<SMEntity*>& v_entity_array = m_Objects[a];

		for (std::size_t b = 0; b < v_entity_array.size(); b++)
			v_entity_array[b]->WriteObjectToFile(v_model, v_offset, transform);
	}
}

void TilePart::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tData) const
{
	for (std::size_t a = 0; a < m_Objects.size(); a++)
	{
		for (const SMEntity* pEntity : m_Objects[a])
			pEntity->FillTextureMap(tData);
	}

	for (std::size_t a = 0; a < m_ClutterMap.size(); a++)
	{
		const SMTileClutter* pClutter = m_ClutterMap[a];
		if (!pClutter) continue;

		pClutter->FillTextureMap(tData);
	}
}

std::size_t TilePart::GetAmountOfObjects() const
{
	std::size_t output = 0;

	for (const std::vector<SMEntity*>& tEntityVec : m_Objects)
	{
		for (const SMEntity* tEntity : tEntityVec)
			output += tEntity->GetAmountOfObjects();
	}

	return output;
}
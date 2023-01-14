#include "Body.hpp"

void SMBody::FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const
{
	for (const SMEntity* v_entity : m_objects)
		v_entity->FillTextureMap(tex_map);
}
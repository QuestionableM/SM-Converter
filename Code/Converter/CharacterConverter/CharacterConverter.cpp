#include "CharacterConverter.hpp"

#include "ObjectDatabase\Mods\Mod.hpp"

#pragma unmanaged

void CharacterConverter::ConvertToModel(const UserCharacterData& v_data, const std::wstring& filename, ConvertError& v_error)
{
	for (std::size_t a = 0; a < (sizeof(v_data.garments) / sizeof(SMUuid)); a++)
		DebugOutL("Test: ", v_data.garments[a].ToString());
}
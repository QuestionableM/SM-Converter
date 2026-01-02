#pragma once

#include "Utils/clr_include.hpp"
#include "Entity.hpp"

SM_UNMANAGED_CODE

class SMGroundTerrainData final : public SMEntity
{
public:
	EntityType Type() const override { return EntityType::GroundTerrain; }

	char* GetMtlNameCStr(const std::string_view& material, const std::size_t idx, char* pCString) const override
	{
		*pCString++ = 'T';
		*pCString++ = 'i';
		*pCString++ = 'l';
		*pCString++ = 'e';
		*pCString++ = 'G';
		*pCString++ = 'r';
		*pCString++ = 'o';
		*pCString++ = 'u';
		*pCString++ = 'n';
		*pCString++ = 'd';
		*pCString++ = 'T';
		*pCString++ = 'e';
		*pCString++ = 'r';
		*pCString++ = 'r';
		*pCString++ = 'a';
		*pCString++ = 'i';
		*pCString++ = 'n';

		return pCString;
	}

	void FillTextureMap(EntityTextureMap& textureMap) const override {}
};

SM_MANAGED_CODE
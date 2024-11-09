#pragma once

#include "Utils/clr_include.hpp"
#include "Entity.hpp"

SM_UNMANAGED_CODE

class SMGroundTerrainData final : public SMEntity
{
public:
	inline EntityType Type() const noexcept override { return EntityType::GroundTerrain; }

	inline char* GetMtlNameCStr(const std::string& v_mat_name, std::size_t v_idx, char* v_ptr) const override
	{
		*v_ptr++ = 'T';
		*v_ptr++ = 'i';
		*v_ptr++ = 'l';
		*v_ptr++ = 'e';
		*v_ptr++ = 'G';
		*v_ptr++ = 'r';
		*v_ptr++ = 'o';
		*v_ptr++ = 'u';
		*v_ptr++ = 'n';
		*v_ptr++ = 'd';
		*v_ptr++ = 'T';
		*v_ptr++ = 'e';
		*v_ptr++ = 'r';
		*v_ptr++ = 'r';
		*v_ptr++ = 'a';
		*v_ptr++ = 'i';
		*v_ptr++ = 'n';

		return v_ptr;
	}

	inline void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tex_map) const override {}
};

SM_MANAGED_CODE
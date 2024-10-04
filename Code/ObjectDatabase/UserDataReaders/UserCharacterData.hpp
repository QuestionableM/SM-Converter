#pragma once

#include "Utils/clr_include.hpp"
#include "Utils/Uuid.hpp"

SM_UNMANAGED_CODE

struct UserCharacterData
{
	UserCharacterData();

	bool from_file(const std::wstring& path);

	unsigned int version;
	bool is_female;

	union
	{
		struct
		{
			SMUuid garment_face;
			SMUuid garment_hair;
			SMUuid garment_facial_hair;
			SMUuid garment_torso;
			SMUuid garment_gloves;
			SMUuid garment_shoes;
			SMUuid garment_pants;
			SMUuid garment_hat;
			SMUuid garment_backpack;
		};

		SMUuid garments[9];
	};

	union
	{
		struct
		{
			unsigned int face_color_idx;
			unsigned int hair_color_idx;
			unsigned int facial_color_idx;
			unsigned int torso_color_idx;
			unsigned int gloves_color_idx;
			unsigned int shoes_color_idx;
			unsigned int pants_color_idx;
			unsigned int hat_color_idx;
			unsigned int backpack_color_idx;
		};

		unsigned int colors[9];
	};
};

SM_MANAGED_CODE
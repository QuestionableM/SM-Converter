#pragma once

#include "UStd/UnmanagedVector.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

namespace Math
{
	inline float lerp(float s, float e, float t)
	{
		return s + (e - s) * t;
	}

	inline float blerp(float c00, float c10, float c01, float c11, float tx, float ty)
	{
		return lerp(lerp(c00, c10, tx), lerp(c01, c11, tx), ty);
	}

	//Requires only 1 size component
	inline float GetHeightPointBox(
		const std::vector<float>& v_height_map,
		std::size_t v_vec_sz,
		std::size_t v_grid_sz,
		float x,
		float y)
	{
		const float v_norm_x = x / float(v_vec_sz) * float(v_grid_sz);
		const float v_norm_y = y / float(v_vec_sz) * float(v_grid_sz);

		const std::size_t v_x_idx = static_cast<std::size_t>(v_norm_x);
		const std::size_t v_y_idx = static_cast<std::size_t>(v_norm_y);
		const std::size_t v_grid_sz_x = v_grid_sz + 1;

		return blerp(
			v_height_map[(v_x_idx    ) + (v_y_idx    ) * v_grid_sz_x],
			v_height_map[(v_x_idx + 1) + (v_y_idx    ) * v_grid_sz_x],
			v_height_map[(v_x_idx    ) + (v_y_idx + 1) * v_grid_sz_x],
			v_height_map[(v_x_idx + 1) + (v_y_idx + 1) * v_grid_sz_x],
			v_norm_x - static_cast<float>(v_x_idx),
			v_norm_y - static_cast<float>(v_y_idx)
		);
	}

	inline float GetHeightPoint(
		const std::vector<float>& vec,
		int vec_width,
		int vec_height,
		int gridSzX,
		int gridSzY,
		float x,
		float y)
	{
		const float gx = x / float(vec_height) * float(gridSzX);
		const float gy = y / float(vec_width) * float(gridSzY);

		const std::size_t gxi = static_cast<std::size_t>(gx);
		const std::size_t gyi = static_cast<std::size_t>(gy);
		const std::size_t v_grid_size_x = static_cast<std::size_t>(gridSzX) + 1;

		const float& c00 = vec[(gxi    ) + (gyi    ) * v_grid_size_x];
		const float& c10 = vec[(gxi + 1) + (gyi    ) * v_grid_size_x];
		const float& c01 = vec[(gxi    ) + (gyi + 1) * v_grid_size_x];
		const float& c11 = vec[(gxi + 1) + (gyi + 1) * v_grid_size_x];

		return blerp(c00, c10, c01, c11, gx - static_cast<float>(gxi), gy - static_cast<float>(gyi));
	}
}

SM_MANAGED_CODE
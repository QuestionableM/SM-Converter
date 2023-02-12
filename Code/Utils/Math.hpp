#pragma once

#include "UStd\UnmanagedVector.hpp"

#pragma unmanaged

namespace Math
{
	inline float lerp(const float& s, const float& e, const float& t)
	{
		return s + (e - s) * t;
	}

	inline float blerp(const float& c00, const float& c10, const float& c01, const float& c11, const float& tx, const float& ty)
	{
		return lerp(lerp(c00, c10, tx), lerp(c01, c11, tx), ty);
	}

	//Requires only 1 size component
	inline float GetHeightPointBox(
		const std::vector<float>& v_height_map,
		const std::size_t& v_vec_sz,
		const std::size_t& v_grid_sz,
		const float& x,
		const float& y)
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
		const int& vec_width,
		const int& vec_height,
		const int& gridSzX,
		const int& gridSzY,
		const float& x,
		const float& y)
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

#pragma managed
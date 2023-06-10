#pragma once

#include "Utils\GlmUnmanaged.hpp"

#pragma unmanaged

namespace Rotations
{
	void InitializeRotations();

	glm::mat4 GetRotationMatrix(unsigned char v_rotation);
	glm::vec3 GetOffsetPosition(unsigned char v_rotation);

	inline unsigned char CompressRotation(int x_axis, int z_axis)
	{
		const unsigned char x_compressed = static_cast<unsigned char>(x_axis + 4) & 0xf;
		const unsigned char z_compressed = static_cast<unsigned char>(z_axis + 4) & 0xf;

		return x_compressed | (z_compressed << 4);
	}

	inline glm::mat4 GetRotationMatrixUncompressed(int x_axis, int z_axis)
	{
		const unsigned char v_rot = Rotations::CompressRotation(x_axis, z_axis);
		return Rotations::GetRotationMatrix(v_rot);
	}

	inline glm::vec3 GetOffsetPositionUncompressed(int x_axis, int z_axis)
	{
		const unsigned char v_rot = Rotations::CompressRotation(x_axis, z_axis);
		return Rotations::GetOffsetPosition(v_rot);
	}
}

#pragma managed
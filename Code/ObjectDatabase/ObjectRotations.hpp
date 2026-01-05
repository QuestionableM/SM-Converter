#pragma once

#include "Utils/GlmUnmanaged.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

namespace Rotations
{
	void InitializeRotations();

	glm::mat4 GetRotationMatrix(unsigned char v_rotation);
	glm::vec3 GetOffsetPosition(unsigned char v_rotation);

	inline std::uint8_t CompressRotation(const int xAxis, const int zAxis)
	{
		const std::uint8_t v_xCompressed = static_cast<std::uint8_t>(xAxis + 4) & 0xf;
		const std::uint8_t v_zCompressed = static_cast<std::uint8_t>(zAxis + 4) & 0xf;

		return v_xCompressed | (v_zCompressed << 4);
	}

	inline glm::mat4 GetRotationMatrixUncompressed(const int xAxis, const int zAxis)
	{
		const std::uint8_t v_rot = Rotations::CompressRotation(xAxis, zAxis);
		return Rotations::GetRotationMatrix(v_rot);
	}

	inline glm::vec3 GetOffsetPositionUncompressed(const int xAxis, const int zAxis)
	{
		const std::uint8_t v_rot = Rotations::CompressRotation(xAxis, zAxis);
		return Rotations::GetOffsetPosition(v_rot);
	}
}

SM_MANAGED_CODE
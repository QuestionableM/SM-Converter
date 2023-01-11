#pragma once

#include <glm.hpp>

namespace Rotations
{
	void InitializeRotations();
	bool RotationExists(const int& xAxis, const int& zAxis);

	glm::mat4 GetRotationMatrix(const int& xAxis, const int& zAxis);
	glm::vec3 GetOffsetPosition(const int& xAxis, const int& zAxis);
}
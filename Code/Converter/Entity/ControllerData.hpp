#pragma once

#include "Utils/GlmUnmanaged.hpp"
#include "Utils/clr_include.hpp"

#include <unordered_map>

SM_UNMANAGED_CODE

struct ControllerPresetData
{
	// joint_id -> angle in degrees (negated if reverse)
	std::unordered_map<std::size_t, float> bearingAngles;
	// controller_id -> extension in notches
	std::unordered_map<std::size_t, float> pistonSettings;
};

struct JointConnection
{
	std::size_t parentBodyIdx;
	std::size_t childBodyIdx;
	glm::mat4 localTransform{ 1.0f }; // identity if no preset
	bool hasPreset = false;
};

SM_MANAGED_CODE

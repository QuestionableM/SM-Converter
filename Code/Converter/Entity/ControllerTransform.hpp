#pragma once

#include "ControllerData.hpp"

#include "Utils/clr_include.hpp"

#include <unordered_map>
#include <vector>

SM_UNMANAGED_CODE

class SMBlueprint;

namespace ControllerTransform
{
	void Apply(
		SMBlueprint* pBlueprint,
		const std::vector<JointConnection>& bodyGraph,
		const std::unordered_map<std::size_t, std::size_t>& childToBodyMap);
}

SM_MANAGED_CODE

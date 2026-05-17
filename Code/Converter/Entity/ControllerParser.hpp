#pragma once

#include "ControllerData.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Json.hpp"

#include <unordered_map>
#include <vector>

SM_UNMANAGED_CODE

namespace ControllerParser
{
	ControllerPresetData ExtractPresets(const simdjson::dom::element& blueprint);

	std::vector<JointConnection> BuildBodyGraph(
		const simdjson::dom::element& blueprint,
		const ControllerPresetData& presetData,
		const std::unordered_map<std::size_t, std::size_t>& childToBodyMap);
}

SM_MANAGED_CODE

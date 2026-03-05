#include "ControllerParser.hpp"

#include "Converter/Entity/Blueprint.hpp"
#include "ObjectDatabase/ObjectRotations.hpp"
#include "Utils/Console.hpp"

SM_UNMANAGED_CODE

ControllerPresetData ControllerParser::ExtractPresets(const simdjson::dom::element& blueprint)
{
	ControllerPresetData v_result;

	const auto v_bodies = blueprint["bodies"];
	if (!v_bodies.is_array()) return v_result;

	for (const auto v_body : v_bodies.get_array().value_unsafe())
	{
		const auto v_childs = v_body["childs"];
		if (!v_childs.is_array()) continue;

		for (const auto v_child : v_childs.get_array().value_unsafe())
		{
			if (!v_child.is_object()) continue;

			const auto v_controller = v_child["controller"];
			if (!v_controller.is_object()) continue;

			// controller.joints[].{id, startAngle, reverse}
			const auto v_joints = v_controller["joints"];
			if (v_joints.is_array())
			{
				for (const auto v_jnt : v_joints.get_array().value_unsafe())
				{
					if (!v_jnt.is_object()) continue;

					const auto v_id = v_jnt["id"];
					const auto v_startAngle = v_jnt["startAngle"];

					if (!v_id.is_number()) continue;

					const std::size_t v_jointId = JsonReader::GetNumber<std::size_t>(v_id.value_unsafe());
					const float v_angle = v_startAngle.is_number()
						? JsonReader::GetNumber<float>(v_startAngle.value_unsafe())
						: 0.0f;

					if (v_angle != 0.0f)
					{
						const auto v_reverse = v_jnt["reverse"];
						const bool v_isReverse = v_reverse.is_bool()
							&& v_reverse.get_bool().value_unsafe();

						v_result.bearingAngles[v_jointId] = v_isReverse ? -v_angle : v_angle;
					}
				}
			}

			// controller.controllers[].{id, frames[0].setting}
			const auto v_controllers = v_controller["controllers"];
			if (v_controllers.is_array())
			{
				for (const auto v_ctrl : v_controllers.get_array().value_unsafe())
				{
					if (!v_ctrl.is_object()) continue;

					const auto v_id = v_ctrl["id"];
					if (!v_id.is_number()) continue;

					const std::size_t v_ctrlId = JsonReader::GetNumber<std::size_t>(v_id.value_unsafe());

					const auto v_frames = v_ctrl["frames"];
					if (!v_frames.is_array()) continue;

					const auto v_framesArray = v_frames.get_array().value_unsafe();
					if (v_framesArray.size() == 0) continue;

					const auto v_firstFrame = *v_framesArray.begin();
					if (!v_firstFrame.is_object()) continue;

					const auto v_setting = v_firstFrame["setting"];
					if (!v_setting.is_number()) continue;

					const float v_settingVal = JsonReader::GetNumber<float>(v_setting.value_unsafe());
					if (v_settingVal != 0.0f)
						v_result.pistonSettings[v_ctrlId] = v_settingVal;
				}
			}
		}
	}

	return v_result;
}

static glm::mat4 ComputeBearingTransform(
	unsigned char xzRotation,
	const glm::vec3& position,
	float angleDegrees)
{
	const glm::mat4 v_rotMat = Rotations::GetRotationMatrix(xzRotation);
	const glm::vec3 v_axis = glm::vec3(v_rotMat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
	const float v_radians = glm::radians(-angleDegrees);

	// Pivot at the center of the bearing's grid cell
	const glm::vec3 v_pivot = position + glm::vec3(0.5f, 0.5f, 0.5f);

	return glm::translate(v_pivot)
		* glm::rotate(v_radians, v_axis)
		* glm::translate(-v_pivot);
}

static glm::mat4 ComputePistonTransform(
	unsigned char xzRotation,
	float settingNotches)
{
	const glm::mat4 v_rotMat = Rotations::GetRotationMatrix(xzRotation);
	const glm::vec3 v_dir = glm::vec3(v_rotMat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
	const float v_offset = settingNotches * 0.25f;

	return glm::translate(v_dir * v_offset);
}

std::vector<JointConnection> ControllerParser::BuildBodyGraph(
	const simdjson::dom::element& blueprint,
	const ControllerPresetData& presetData,
	const std::unordered_map<std::size_t, std::size_t>& childToBodyMap)
{
	std::vector<JointConnection> v_result;

	const auto v_joints = blueprint["joints"];
	if (!v_joints.is_array()) return v_result;

	const auto v_jointsArray = v_joints.get_array().value_unsafe();
	v_result.reserve(v_jointsArray.size());

	for (const auto v_jnt : v_jointsArray)
	{
		if (!v_jnt.is_object()) continue;

		const auto v_childA = v_jnt["childA"];
		const auto v_childB = v_jnt["childB"];
		const auto v_id = v_jnt["id"];

		if (!(v_childA.is_number() && v_childB.is_number() && v_id.is_number()))
			continue;

		const std::size_t v_childAIdx = JsonReader::GetNumber<std::size_t>(v_childA.value_unsafe());
		const std::size_t v_childBIdx = JsonReader::GetNumber<std::size_t>(v_childB.value_unsafe());

		const auto v_parentIt = childToBodyMap.find(v_childAIdx);
		const auto v_childIt = childToBodyMap.find(v_childBIdx);
		if (v_parentIt == childToBodyMap.end() || v_childIt == childToBodyMap.end())
			continue;

		if (v_parentIt->second == v_childIt->second)
			continue;

		JointConnection v_conn;
		v_conn.parentBodyIdx = v_parentIt->second;
		v_conn.childBodyIdx = v_childIt->second;

		const std::size_t v_jointId = JsonReader::GetNumber<std::size_t>(v_id.value_unsafe());

		const auto v_xAxis = v_jnt["xaxisA"];
		const auto v_zAxis = v_jnt["zaxisA"];
		const int v_xAxisInt = v_xAxis.is_number() ? JsonReader::GetNumber<int>(v_xAxis.value_unsafe()) : 1;
		const int v_zAxisInt = v_zAxis.is_number() ? JsonReader::GetNumber<int>(v_zAxis.value_unsafe()) : 3;
		const unsigned char v_xzRotation = Rotations::CompressRotation(v_xAxisInt, v_zAxisInt);

		const auto v_bearingIt = presetData.bearingAngles.find(v_jointId);
		if (v_bearingIt != presetData.bearingAngles.end())
		{
			const glm::vec3 v_posVec = SMBlueprint::JsonToVector(v_jnt["posA"]);
			v_conn.localTransform = ComputeBearingTransform(v_xzRotation, v_posVec, v_bearingIt->second);
			v_conn.hasPreset = true;
			v_result.push_back(v_conn);
			continue;
		}

		// Piston presets are matched via the joint's own controller.id
		const auto v_jntController = v_jnt["controller"];
		if (v_jntController.is_object())
		{
			const auto v_ctrlId = v_jntController["id"];
			if (v_ctrlId.is_number())
			{
				const std::size_t v_controllerId = JsonReader::GetNumber<std::size_t>(v_ctrlId.value_unsafe());
				const auto v_pistonIt = presetData.pistonSettings.find(v_controllerId);
				if (v_pistonIt != presetData.pistonSettings.end())
				{
					v_conn.localTransform = ComputePistonTransform(v_xzRotation, v_pistonIt->second);
					v_conn.hasPreset = true;
					v_result.push_back(v_conn);
					continue;
				}
			}
		}

		// Still needed for hierarchy traversal even without a preset
		v_result.push_back(v_conn);
	}

	return v_result;
}

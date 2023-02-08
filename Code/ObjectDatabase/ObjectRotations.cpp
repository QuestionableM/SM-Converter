#include "ObjectRotations.hpp"

#include "ObjectDatabase\DatabaseConfig.hpp"

#include "UStd\UnmanagedUnorderedMap.hpp"
#include "UStd\UnmanagedString.hpp"

#include "Utils\Console.hpp"
#include "Utils\Json.hpp"

#pragma unmanaged

namespace Rotations
{
	struct RotationEntry
	{
		glm::mat4 rot_matrix;
		glm::vec3 pos_vector;
	};

	static std::unordered_map<unsigned char, RotationEntry> RotationTable = {};
}

void Rotations::InitializeRotations()
{
	const nlohmann::json v_settings = JsonReader::LoadParseJson(DatabaseConfig::RotationsPath.data());
	if (!v_settings.is_object())
	{
		DebugErrorL("Couldn't read rotation configuration file!");
		return;
	}

	DebugOutL("Loading rotations from file...");

	const auto& v_rotations = JsonReader::Get(v_settings, "Rotations");
	if (!v_rotations.is_object()) return;

	for (const auto& v_first_rot : v_rotations.items())
	{
		if (!v_first_rot.value().is_object()) continue;

		const int v_x_axis_rot = std::stoi(v_first_rot.key());

		for (const auto& v_second_rot : v_first_rot.value().items())
		{
			if (!v_second_rot.value().is_object()) continue;

			const int v_z_axis_rot = std::stoi(v_second_rot.key());
			const char v_rot_compressed = Rotations::CompressRotation(v_x_axis_rot, v_z_axis_rot);

			RotationEntry v_entry;
			v_entry.pos_vector = glm::vec3(0.0f);
			v_entry.rot_matrix = glm::mat4(1.0f);

			const auto v_offset = JsonReader::Get(v_second_rot.value(), "Offset");
			if (v_offset.is_object())
			{
				const auto& v_x_pos = JsonReader::Get(v_offset, "x");
				const auto& v_y_pos = JsonReader::Get(v_offset, "y");
				const auto& v_z_pos = JsonReader::Get(v_offset, "z");

				if (v_x_pos.is_number() && v_y_pos.is_number() && v_z_pos.is_number())
					v_entry.pos_vector = glm::vec3(v_x_pos.get<float>(), v_y_pos.get<float>(), v_z_pos.get<float>());
			}

			const auto v_rot_mat = JsonReader::Get(v_second_rot.value(), "Rotation");
			if (v_rot_mat.is_object())
			{
				const auto& v_x_rot = JsonReader::Get(v_rot_mat, "x");
				const auto& v_y_rot = JsonReader::Get(v_rot_mat, "y");
				const auto& v_z_rot = JsonReader::Get(v_rot_mat, "z");

				if (v_x_rot.is_number() && v_y_rot.is_number() && v_z_rot.is_number())
				{
					v_entry.rot_matrix = glm::rotate(v_entry.rot_matrix, glm::radians(v_x_rot.get<float>()), glm::vec3(1.0f, 0.0f, 0.0f));
					v_entry.rot_matrix = glm::rotate(v_entry.rot_matrix, glm::radians(v_y_rot.get<float>()), glm::vec3(0.0f, 1.0f, 0.0f));
					v_entry.rot_matrix = glm::rotate(v_entry.rot_matrix, glm::radians(v_z_rot.get<float>()), glm::vec3(0.0f, 0.0f, 1.0f));
				}
			}

			Rotations::RotationTable.insert(std::make_pair(v_rot_compressed, v_entry));
		}
	}

	DebugOutL(0b0101_fg, "Rotations have been initialized!");
}

glm::mat4 Rotations::GetRotationMatrix(const unsigned char& v_rotation)
{
	const std::unordered_map<unsigned char, RotationEntry>::const_iterator v_iter = Rotations::RotationTable.find(v_rotation);
	if (v_iter == Rotations::RotationTable.end())
		return glm::mat4(1.0f);

	return v_iter->second.rot_matrix;
}

glm::vec3 Rotations::GetOffsetPosition(const unsigned char& v_rotation)
{
	const std::unordered_map<unsigned char, RotationEntry>::const_iterator v_iter = Rotations::RotationTable.find(v_rotation);
	if (v_iter == Rotations::RotationTable.end())
		return glm::vec3(0.0f);

	return v_iter->second.pos_vector;
}
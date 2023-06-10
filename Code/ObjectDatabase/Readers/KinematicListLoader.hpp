#pragma once

#include "Utils\Json.hpp"

#pragma unmanaged

class SMMod;

class KinematicListLoader
{
	KinematicListLoader() = default;

public:
	static void Load(const simdjson::dom::element& v_kinematics, SMMod* mod, bool add_to_global_db);
};

#pragma managed
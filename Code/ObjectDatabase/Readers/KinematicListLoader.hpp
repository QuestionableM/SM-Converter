#pragma once

#include "Utils/clr_include.hpp"
#include "Utils/Json.hpp"

SM_UNMANAGED_CODE

class SMMod;

class KinematicListLoader
{
	KinematicListLoader() = default;

public:
	static void Load(const simdjson::dom::element& v_kinematics, SMMod* mod, bool add_to_global_db);
};

SM_MANAGED_CODE
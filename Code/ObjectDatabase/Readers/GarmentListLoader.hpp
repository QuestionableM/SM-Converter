#pragma once

#include "Utils/clr_include.hpp"
#include "Utils/Json.hpp"

SM_UNMANAGED_CODE

class SMMod;

class GarmentListLoader
{
public:
	static void Load(const simdjson::dom::element& v_garments, SMMod* mod, bool add_to_global_db);

private:
	static void LoadGarmentCategory(const simdjson::dom::element& v_category);

	GarmentListLoader() = default;
	GarmentListLoader(const GarmentListLoader&) = delete;
	GarmentListLoader(GarmentListLoader&&) = delete;
	~GarmentListLoader() = default;
};

SM_MANAGED_CODE
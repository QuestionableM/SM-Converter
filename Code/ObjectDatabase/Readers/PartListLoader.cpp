#include "PartListLoader.hpp"

#include "ObjectDatabase\Readers\DefaultLoader.hpp"
#include "ObjectDatabase\ProgCounter.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\Console.hpp"

#pragma unmanaged

const PartListLoader::__CollisionLoaderData PartListLoader::g_collisionDataLoaders[] =
{
	{ "box"     , PartListLoader::LoadBoxCollision      },
	{ "hull"    , PartListLoader::LoadBoxCollision      },
	{ "cylinder", PartListLoader::LoadCylinderCollision },
	{ "sphere"  , PartListLoader::LoadSphereCollision   },
	{ "slant"   , PartListLoader::LoadBoxCollision      }
};

void PartListLoader::LoadBoxCollision(const simdjson::dom::element& collision, glm::vec3& vec_ref)
{
	const auto v_coll_x = collision["x"];
	const auto v_coll_y = collision["y"];
	const auto v_coll_z = collision["z"];

	if (v_coll_x.is_number() && v_coll_y.is_number() && v_coll_z.is_number())
	{
		vec_ref = glm::vec3(
			JsonReader::GetNumber<float>(v_coll_x.value_unsafe()),
			JsonReader::GetNumber<float>(v_coll_y.value_unsafe()),
			JsonReader::GetNumber<float>(v_coll_z.value_unsafe())
		);
	}
}

void PartListLoader::LoadCylinderCollision(const simdjson::dom::element& collision, glm::vec3& vec_ref)
{
	const auto v_diameter = collision["diameter"];
	const auto v_depth = collision["depth"];

	if (!(v_diameter.is_number() && v_depth.is_number()))
		return;

	char v_axis_char = 'z';

	const auto v_axis = collision["axis"];
	if (v_axis.is_string())
	{
		const std::string_view v_axis_str = v_axis.get_string().value_unsafe();
		if (v_axis_str.size() > 0)
			v_axis_char = std::tolower(v_axis_str.data()[0]);
	}

	const float v_diameter_f = JsonReader::GetNumber<float>(v_diameter.value_unsafe());
	const float v_depth_f = JsonReader::GetNumber<float>(v_depth.value_unsafe());

	switch (v_axis_char)
	{
	case 'x': vec_ref = glm::vec3(v_depth_f   , v_diameter_f, v_diameter_f); break;
	case 'y': vec_ref = glm::vec3(v_diameter_f, v_depth_f   , v_diameter_f); break;
	case 'z': vec_ref = glm::vec3(v_diameter_f, v_diameter_f, v_depth_f   ); break;
	}
}

void PartListLoader::LoadSphereCollision(const simdjson::dom::element& collision, glm::vec3& vec_ref)
{
	const auto v_diameter = collision["diameter"];
	if (v_diameter.is_number())
		vec_ref = glm::vec3(JsonReader::GetNumber<float>(v_diameter.value_unsafe()));
}

glm::vec3 PartListLoader::LoadPartCollision(const simdjson::dom::element& collision)
{
	glm::vec3 v_out_collision(1.0f);

	const unsigned __int8 v_collisionDataLoadersSz = sizeof(g_collisionDataLoaders) / sizeof(__CollisionLoaderData);
	for (unsigned __int8 a = 0; a < v_collisionDataLoadersSz; a++)
	{
		const __CollisionLoaderData& v_curData = g_collisionDataLoaders[a];

		const auto v_collision_data = collision[v_curData.key];
		if (!v_collision_data.is_object())
			continue;

		v_curData.func_ptr(v_collision_data.value_unsafe(), v_out_collision);
		break;
	}

	return v_out_collision;
}

void PartListLoader::Load(const simdjson::dom::element& fParts, SMMod* mod, bool add_to_global_db)
{
	if (!fParts.is_array()) return;

	const auto v_prt_array = fParts.get_array().value_unsafe();
	ProgCounter::ProgressMax += v_prt_array.size();

	auto& v_cur_db = mod->m_Parts.GetStorage(add_to_global_db);
	auto v_adder_func = mod->m_Parts.GetAdderFunction(add_to_global_db);

	std::wstring v_meshPath;

	for (const auto v_prt : v_prt_array)
	{
		if (!v_prt.is_object()) continue;

		const auto v_uuid = v_prt["uuid"];
		const auto v_color = v_prt["color"];

		if (!v_uuid.is_string()) continue;

		const SMUuid v_prtUuid = v_uuid.get_string().value_unsafe();
		if (mod->m_Parts.ObjectExists(v_cur_db, v_prtUuid))
			continue;

		SMSubMeshBase* v_pSubMesh;
		if (!DefaultLoader::LoadRenderable(v_prt, &v_pSubMesh, v_meshPath))
			continue;

		PartData* v_new_part = new PartData(
			v_prtUuid,
			std::move(v_meshPath),
			v_pSubMesh,
			v_color.is_string() ? v_color.get_string().value_unsafe() : "375000",
			PartListLoader::LoadPartCollision(v_prt),
			mod
		);

		(mod->m_Parts.*v_adder_func)(v_new_part);
		ProgCounter::ProgressValue++;
	}
}
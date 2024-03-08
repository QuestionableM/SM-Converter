#pragma once

#include "Utils/GlmUnmanaged.hpp"
#include "Utils/clr_include.hpp"
#include "Utils/Json.hpp"
#include "Utils/Uuid.hpp"

SM_UNMANAGED_CODE

class SMMod;

class PartListLoader
{
	PartListLoader() = default;
	~PartListLoader() = default;

	static void LoadBoxCollision(const simdjson::dom::element& collision, glm::vec3& vec_ref);
	static void LoadCylinderCollision(const simdjson::dom::element& collision, glm::vec3& vec_ref);
	static void LoadSphereCollision(const simdjson::dom::element& collision, glm::vec3& vec_ref);

	static glm::vec3 LoadPartCollision(const simdjson::dom::element& collision);

public:
	static void Load(const simdjson::dom::element& fParts, SMMod* mod, bool add_to_global_db);

private:
	struct __CollisionLoaderData
	{
		std::string key;
		void (*func_ptr)(const simdjson::dom::element&, glm::vec3&);
	};

	static const __CollisionLoaderData g_collisionDataLoaders[];
};

SM_MANAGED_CODE
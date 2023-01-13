#pragma once

#include "Utils\Json.hpp"

#include <glm.hpp>

class Mod;

class PartListLoader
{
	PartListLoader() = default;
	~PartListLoader() = default;

	static void LoadBoxCollision(const simdjson::dom::element& collision, glm::vec3& vec_ref);
	static void LoadCylinderCollision(const simdjson::dom::element& collision, glm::vec3& vec_ref);
	static void LoadSphereCollision(const simdjson::dom::element& collision, glm::vec3& vec_ref);

	static glm::vec3 LoadPartCollision(const simdjson::dom::element& collision);

public:
	static void Load(const simdjson::dom::element& fParts, Mod* mod);

private:
	struct __CollisionLoaderData
	{
		std::string key;
		void (*func_ptr)(const simdjson::dom::element&, glm::vec3&);
	};

	static const __CollisionLoaderData g_collisionDataLoaders[];
};
#pragma once

#include "Utils/GlmUnmanaged.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

#include <vcruntime.h>

struct WriterOffsetData
{
	using Vec3Iterator = typename std::unordered_map<glm::vec3, std::size_t>::const_iterator;
	using Vec2Iterator = typename std::unordered_map<glm::vec2, std::size_t>::const_iterator;

	std::unordered_map<glm::vec3, std::size_t> VertexMap = {};
	std::unordered_map<glm::vec3, std::size_t> NormalMap = {};
	std::unordered_map<glm::vec2, std::size_t> UvMap     = {};

	std::size_t Vertex = 0;
	std::size_t Normal = 0;
	std::size_t Uv = 0;

	WriterOffsetData() = default;
	WriterOffsetData(const WriterOffsetData&) = delete;
	WriterOffsetData(WriterOffsetData&&) = delete;
	~WriterOffsetData() = default;
};

SM_MANAGED_CODE
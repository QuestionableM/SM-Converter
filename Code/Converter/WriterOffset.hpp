#pragma once

#include "Utils\GlmUnmanaged.hpp"

#include <vcruntime.h>

#pragma unmanaged

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
	WriterOffsetData(WriterOffsetData&) = delete;
	~WriterOffsetData() = default;
};

#pragma managed
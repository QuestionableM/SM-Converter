#pragma once

#include "Utils/GlmUnmanaged.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

#include <hash_table8.hpp>

#include <cstdint>

struct WriterOffsetData
{
	emhash8::HashMap<glm::vec3, std::size_t> VertexMap = {};
	emhash8::HashMap<glm::vec3, std::size_t> NormalMap = {};
	emhash8::HashMap<glm::vec2, std::size_t> UvMap = {};
	//std::unordered_map<glm::vec3, std::size_t> VertexMap = {};
	//std::unordered_map<glm::vec3, std::size_t> NormalMap = {};
	//std::unordered_map<glm::vec2, std::size_t> UvMap     = {};

	std::size_t Vertex = 1;
	std::size_t Normal = 1;
	std::size_t Uv = 1;

	WriterOffsetData() = default;
	WriterOffsetData(const WriterOffsetData&) = delete;
	WriterOffsetData(WriterOffsetData&&) = delete;
	~WriterOffsetData() = default;
};

SM_MANAGED_CODE
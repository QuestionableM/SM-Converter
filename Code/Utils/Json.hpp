#pragma once

#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

#include <nlohmann/json.hpp>

#if defined(_MT)
	#undef _MT
	#include <simdjson/simdjson.h>
	#define _MT
#else
	#include <simdjson/simdjson.h>
#endif

class JsonReader
{
public:
	static nlohmann::json ParseJsonString(const std::string& json_str);
	static bool LoadParseJson(const std::wstring_view& path, nlohmann::json& outJson);

	static void WriteJson(const std::wstring_view& path, const nlohmann::json& pJson);
	static std::string WriteJsonString(const nlohmann::json& v_json);

	static const nlohmann::json& Get(const nlohmann::json& obj, const std::string_view& key);
	static const nlohmann::json& Get(const nlohmann::json& obj, std::size_t key);

	//Simdjson functions

	//Only used by Simdjson as it doesn't support json with comments
	static void RemoveComments(std::string& json_string);

	//Parse simdjson unchecked
	static bool LoadParseSimdjson(const std::wstring_view& path, simdjson::dom::document& v_doc);
	//Parse simdjson with root node type check
	static bool LoadParseSimdjsonC(const std::wstring_view& path, simdjson::dom::document& v_doc, const simdjson::dom::element_type& type_check);
	//Parse simdjson with comments
	static bool LoadParseSimdjsonComments(const std::wstring_view& path, simdjson::dom::document& v_doc);
	//Parse simdjson with comments and root node type checking
	static bool LoadParseSimdjsonCommentsC(const std::wstring_view& path, simdjson::dom::document& v_doc, const simdjson::dom::element_type& type_check);
	//Parse simdjson from string
	static bool ParseSimdjsonString(const std::string_view& json_str, simdjson::dom::document& v_doc);



	//Should be used to get numbers from simdjson elements
	template<typename T>
	inline constexpr static T GetNumber(const simdjson::dom::element& v_elem)
	{
		switch (v_elem.type())
		{
		case simdjson::dom::element_type::DOUBLE:
		{
			if constexpr (std::is_same_v<T, double>)
				return v_elem.get_double().value_unsafe();
			else
				return static_cast<T>(v_elem.get_double().value_unsafe());
		}
		case simdjson::dom::element_type::INT64:
		{
			if constexpr (std::is_same_v<T, long long>)
				return v_elem.get_int64().value_unsafe();
			else
				return static_cast<T>(v_elem.get_int64().value_unsafe());
		}
		case simdjson::dom::element_type::UINT64:
		{
			if constexpr (std::is_same_v<T, unsigned long long>)
				return v_elem.get_uint64().value_unsafe();
			else
				return static_cast<T>(v_elem.get_uint64().value_unsafe());
		}
		default:
			return 0;
		}
	}

private:
	inline const static nlohmann::json m_emptyObject = nlohmann::json();

	JsonReader() = default;
	JsonReader(const JsonReader&) = delete;
	JsonReader(JsonReader&&) = delete;
	~JsonReader() = default;
};

SM_MANAGED_CODE
#include "Json.hpp"

#include "Utils\String.hpp"
#include "Utils\File.hpp"
#include "Console.hpp"

#include <fstream>
#include <iomanip>

nlohmann::json JsonReader::ParseJsonString(const std::string& json_str)
{
	try
	{
		return nlohmann::json::parse(json_str, nullptr, true, true);
	}
#ifdef _DEBUG
	catch (nlohmann::json::parse_error& p_err)
	{
		DebugErrorL("Couldn't parse the specified json string: ", json_str, "\nError: ", p_err.what());
	}
#else
	catch (...) {}
#endif

	return m_emptyObject;
}

nlohmann::json JsonReader::LoadParseJson(const std::wstring& path)
{
	std::string v_fileData;
	if (!File::ReadToString(path, v_fileData))
	{
		DebugErrorL("Couldn't read the specified json file: ", path);
		return m_emptyObject;
	}

	try
	{
		return nlohmann::json::parse(v_fileData, nullptr, true, true);
	}
#ifdef _DEBUG
	catch (nlohmann::json::parse_error& p_err)
	{
		DebugErrorL("Couldn't parse the json file:\nPath: ", path, "\nError: ", p_err.what());
	}
#else
	catch (...) {}
#endif

	return m_emptyObject;
}

bool JsonReader::LoadParseJsonLua(const std::wstring& path, nlohmann::json& v_json, std::string& v_error_message)
{
	std::string v_fileData;
	if (!File::ReadToString(path, v_fileData))
	{
		v_error_message = "Failed to open file";
		return false;
	}

	try
	{
		v_json = nlohmann::json::parse(v_fileData, nullptr, true, true);
		return true;
	}
	catch (nlohmann::json::parse_error& p_err)
	{
		v_error_message = "Parse error: " + std::string(p_err.what());
	}
	catch (...)
	{
		v_error_message = "Unknown json error";
	}

	return false;
}

bool JsonReader::ParseJsonStringLua(const std::string& json_str, nlohmann::json& v_json, std::string& v_error_message)
{
	try
	{
		v_json = nlohmann::json::parse(json_str, nullptr, true, true);
		return true;
	}
	catch (nlohmann::json::parse_error& p_err)
	{
		v_error_message = "Parse error: " + std::string(p_err.what());
	}
	catch (...)
	{
		v_error_message = "Unknown parse error";
	}


	return false;
}

void JsonReader::WriteJson(const std::wstring& path, const nlohmann::json& pJson)
{
	std::ofstream out_file(path);
	if (!out_file.is_open()) return;

	out_file << std::setw(1) << std::setfill('\t') << pJson;
	out_file.close();
}

std::string JsonReader::WriteJsonString(const nlohmann::json& v_json)
{
	std::stringstream v_str;
	v_str << std::setw(0) << v_json;

	return v_str.str();
}

//Simdjson functions

std::string JsonReader::RemoveComments(const std::string& json_string)
{
	std::string v_output;
	v_output.reserve(json_string.size());

	const char* const v_data_beg = json_string.data();
	const char* const v_data_end = v_data_beg + json_string.size();

	const char* v_data = v_data_beg;

	std::size_t v_data_ptr = 0;
	while (v_data != v_data_end)
	{
		if (*v_data == '\"')
		{
			v_data = strchr(v_data + 1, '\"');
			if (!v_data) break;
		}
		else if (*v_data == '/')
		{
			const char* v_last_char = v_data++;
			if (v_data == v_data_end)
			{
				break;
			}
			else if (*v_data == '/')
			{
				v_output.append(json_string.begin() + v_data_ptr, json_string.begin() + (v_last_char - v_data_beg));

				v_data = strchr(v_data, '\n');
				if (!v_data) break;

				v_data_ptr = v_data - v_data_beg;
				continue;
			}
			else if (*v_data == '*')
			{
				v_output.append(json_string.begin() + v_data_ptr, json_string.begin() + (v_last_char - v_data_beg));

				v_data = strstr(v_data, "*/");
				if (!v_data) break;

				v_data_ptr = (v_data += 2) - v_data_beg;
				continue;
			}
		}

		v_data++;
	}

	if (v_data)
	{
		v_output.append(json_string.begin() + v_data_ptr, json_string.begin() + (v_data - v_data_beg));
	}
	else
	{
		v_output.append(json_string.substr(v_data_ptr));
	}

	return v_output;
}

bool JsonReader::LoadParseSimdjson(const std::wstring& path, simdjson::dom::document& v_doc)
{
	try
	{
		std::string v_json_str;
		if (!File::ReadToString(path, v_json_str))
			return false;

		simdjson::dom::parser v_parser;
		v_parser.parse_into_document(v_doc, v_json_str);

		return true;
	}
#if defined(_DEBUG) || defined(DEBUG)
	catch (const simdjson::simdjson_error& v_err)
	{
		DebugErrorL("Couldn't parse: ", path, "\nError: ", v_err.what());
	}
#else
	catch (...) {}
#endif

	return false;
}

bool JsonReader::LoadParseSimdjsonC(const std::wstring& path, simdjson::dom::document& v_doc, const simdjson::dom::element_type& type_check)
{
	try
	{
		std::string v_json_str;
		if (!File::ReadToString(path, v_json_str))
			return false;

		simdjson::dom::parser v_parser;
		const auto v_root = v_parser.parse_into_document(v_doc, v_json_str);
		if (v_root.type() != type_check)
		{
			DebugErrorL("Mismatching root json type!\nFile: ", path);
			return false;
		}

		return true;
	}
#if defined(_DEBUG) || defined(DEBUG)
	catch (const simdjson::simdjson_error& v_err)
	{
		DebugErrorL("Couldn't parse: ", path, "\nError: ", v_err.what());
	}
#else
	catch (...) {}
#endif

	return false;
}

bool JsonReader::LoadParseSimdjsonComments(const std::wstring& path, simdjson::dom::document& v_doc)
{
	try
	{
		std::string v_json_str;
		if (!File::ReadToString(path, v_json_str))
			return false;

		v_json_str = JsonReader::RemoveComments(v_json_str);

		simdjson::dom::parser v_parser;
		v_parser.parse_into_document(v_doc, v_json_str);

		return true;
	}
#if defined(_DEBUG) || defined(DEBUG)
	catch (const simdjson::simdjson_error& v_err)
	{
		DebugErrorL("Couldn't parse: ", path, "\nError: ", v_err.what());
	}
#else
	catch (...) {}
#endif

	return false;
}

bool JsonReader::LoadParseSimdjsonCommentsC(const std::wstring& path, simdjson::dom::document& v_doc, const simdjson::dom::element_type& type_check)
{
	try
	{
		std::string v_json_str;
		if (!File::ReadToString(path, v_json_str))
			return false;

		v_json_str = JsonReader::RemoveComments(v_json_str);

		simdjson::dom::parser v_parser;
		const auto v_root = v_parser.parse_into_document(v_doc, v_json_str);
		if (v_root.type() != type_check)
		{
			DebugErrorL("Mismatching root json type!\nFile: ", path);
			return false;
		}

		return true;
	}
#if defined(_DEBUG) || defined(DEBUG)
	catch (const simdjson::simdjson_error& v_err)
	{
		DebugErrorL("Couldn't parse: ", path, "\nError: ", v_err.what());
	}
#else
	catch (...) {}
#endif

	return false;
}

bool JsonReader::ParseSimdjsonString(const std::string& json_str, simdjson::dom::document& v_doc)
{
	try
	{
		simdjson::dom::parser v_parser;
		v_parser.parse_into_document(v_doc, json_str);

		return true;
	}
#if defined(_DEBUG) || defined(DEBUG)
	catch (const simdjson::simdjson_error& v_err)
	{
		DebugErrorL("Couldn't parse a json string. Error: ", v_err.what());
	}
#else
	catch (...) {}
#endif

	return false;
}

bool JsonReader::LoadParseSimdjsonLua(const std::wstring& path, simdjson::dom::document& v_doc, std::string& v_error_message)
{
	std::string v_json_str;
	if (!File::ReadToString(path, v_json_str))
	{
		v_error_message = "Failed to open file";
		return false;
	}

	v_json_str = JsonReader::RemoveComments(v_json_str);

	try
	{
		simdjson::dom::parser v_parser;
		v_parser.parse_into_document(v_doc, v_json_str);

		return true;
	}
	catch (const simdjson::simdjson_error& v_err)
	{
		v_error_message = "Parse error: " + std::string(v_err.what());
	}
	catch (...)
	{
		v_error_message = "Unknown json error";
	}

	return false;
}

bool JsonReader::ParseSimdjsonStringLua(const std::string& json_str, simdjson::dom::document& v_doc, std::string& v_error_message)
{
	try
	{
		simdjson::dom::parser v_parser;
		v_parser.parse_into_document(v_doc, json_str);

		return true;
	}
	catch (const simdjson::simdjson_error& v_err)
	{
		v_error_message = "Parse error: " + std::string(v_err.what());
	}
	catch (...)
	{
		v_error_message = "Unknown parse error";
	}

	return false;
}
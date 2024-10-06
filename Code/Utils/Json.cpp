#include "Json.hpp"

#include "UStd\UnmanagedFstream.hpp"
#include "UStd\UnmanagedIomanip.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\File.hpp"


#pragma unmanaged

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

void JsonReader::RemoveComments(std::string& json_string)
{
	char* const v_dataBeg = json_string.data();
	char* const v_dataEnd = v_dataBeg + json_string.size();
	char* v_data = v_dataBeg;

	bool v_brFlag = false;
	std::size_t v_curlyBrCount = 0;
	std::size_t v_brCount = 0;

	while (v_data < v_dataEnd)
	{
		switch (*v_data)
		{
		case '{':
		{
			if (!v_brFlag) { v_brFlag = true; break; }
			v_curlyBrCount++;
			break;
		}
		case '[':
		{
			if (!v_brFlag) { v_brFlag = true; break; }
			v_brCount++;
			break;
		}
		case '}':
		{
			if (v_curlyBrCount <= 0)
			{
				v_data++;
				std::memset(v_data, ' ', v_dataEnd - v_data);
				return;
			}

			v_curlyBrCount--;
			break;
		}
		case ']':
		{
			if (v_brCount <= 0)
			{
				v_data++;
				std::memset(v_data, ' ', v_dataEnd - v_data);
				return;
			}

			v_brCount--;
			break;
		}
		case '\"':
		{
			// Find the good string end character
			const char* v_strEnd = v_data + 1;
			for (; v_strEnd < v_dataEnd; v_strEnd++)
			{
				if (*v_strEnd == '\"' && *(v_strEnd - 1) != '\\')
					break;
			}

			// Remove the newline characters from json strings to avoid parse errors
			for (; v_data < v_strEnd; v_data++)
			{
				if (*v_data == '\n' || *v_data == '\r')
					*v_data = ' ';
			}

			break;
		}
		case '/':
		{
			char* const v_lastChar = v_data++;
			if (v_data == v_dataEnd) return;

			switch (*v_data)
			{
			case '/':
			{
				v_data = strchr(v_data + 1, '\n');
				if (!v_data) return;

				v_data++;
				std::memset(v_lastChar, ' ', v_data - v_lastChar);
				continue;
			}
			case '*':
			{
				v_data = strstr(v_data + 1, "*/");
				if (!v_data) return;

				v_data += 2;
				std::memset(v_lastChar, ' ', v_data - v_lastChar);
				continue;
			}
			default:
				break;
			}

			break;
		}
		default:
			break;
		}

		v_data++;
	}
}

bool JsonReader::LoadParseSimdjson(const std::wstring& path, simdjson::dom::document& v_doc)
{
	try
	{
		std::string v_json_str;
		if (!File::ReadToStringED(path, v_json_str))
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
		if (!File::ReadToStringED(path, v_json_str))
			return false;

		simdjson::dom::parser v_parser;
		v_parser.parse_into_document(v_doc, v_json_str);

		const auto v_root = v_doc.root();
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
		if (!File::ReadToStringED(path, v_json_str))
			return false;

		JsonReader::RemoveComments(v_json_str);

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
		if (!File::ReadToStringED(path, v_json_str))
			return false;

		JsonReader::RemoveComments(v_json_str);

		simdjson::dom::parser v_parser;
		const auto v_root = v_parser.parse_into_document(v_doc, v_json_str);
		if (v_root.error())
		{
			DebugErrorL("Parse Error:\nFile: ", path, "\nError: ", simdjson::error_message(v_root.error()));
			return false;
		}

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

bool JsonReader::ParseSimdjsonString(const std::string_view& json_str, simdjson::dom::document& v_doc)
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
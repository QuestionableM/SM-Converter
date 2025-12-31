#include "Json.hpp"

#include "UStd\UnmanagedFstream.hpp"
#include "UStd\UnmanagedIomanip.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\File.hpp"

SM_UNMANAGED_CODE

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

bool JsonReader::LoadParseJson(const std::wstring_view& path, nlohmann::json& outJson)
{
	std::string v_fileData;
	bool v_success = false;

	if (File::ReadToString(path, v_fileData))
	{
		try
		{
			outJson = nlohmann::json::parse(v_fileData, nullptr, true, true);
			v_success = true;
		}
	#ifdef _DEBUG
		catch (nlohmann::json::parse_error& p_err)
		{
			DebugErrorL("Couldn't parse the json file:\nPath: ", path, "\nError: ", p_err.what());
		}
	#else
		catch (...) {}
	#endif
	}
	else
	{
		DebugErrorL("Couldn't read the specified json file: ", path);
	}

	return v_success;
}

void JsonReader::WriteJson(const std::wstring_view& path, const nlohmann::json& pJson)
{
	std::ofstream out_file(path.data());
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

const nlohmann::json& JsonReader::Get(const nlohmann::json& obj, const std::string_view& key)
{
	const auto v_iter = obj.find(key);
	if (v_iter != obj.end())
		return v_iter.value();

	return m_emptyObject;
}

const nlohmann::json& JsonReader::Get(const nlohmann::json& obj, std::size_t key)
{
	if (key < obj.size())
		return obj.at(key);

	return m_emptyObject;
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

bool JsonReader::LoadParseSimdjson(const std::wstring_view& path, simdjson::dom::document& v_doc)
{
	std::string v_jsonStr;
	bool v_success = false;

	if (File::ReadToStringED(path, v_jsonStr))
	{
		try
		{
			simdjson::dom::parser v_parser;
			v_parser.parse_into_document(v_doc, v_jsonStr);

			v_success = true;
		}
	#if defined(_DEBUG) || defined(DEBUG)
		catch (const simdjson::simdjson_error& v_err)
		{
			DebugErrorL("Couldn't parse: ", path, "\nError: ", v_err.what());
		}
	#else
		catch (...) {}
	#endif
	}

	return v_success;
}

bool JsonReader::LoadParseSimdjsonC(const std::wstring_view& path, simdjson::dom::document& v_doc, const simdjson::dom::element_type& type_check)
{
	std::string v_jsonStr;
	bool v_success = false;

	if (File::ReadToStringED(path, v_jsonStr))
	{
		try
		{
			simdjson::dom::parser v_parser;
			v_parser.parse_into_document(v_doc, v_jsonStr);

			if (v_doc.root().type() == type_check)
				v_success = true;
			else
				DebugErrorL("Mismatching root json type!\nFile: ", path);
		}
	#if defined(_DEBUG) || defined(DEBUG)
		catch (const simdjson::simdjson_error& v_err)
		{
			DebugErrorL("Couldn't parse: ", path, "\nError: ", v_err.what());
		}
	#else
		catch (...) {}
	#endif
	}


	return v_success;
}

bool JsonReader::LoadParseSimdjsonComments(const std::wstring_view& path, simdjson::dom::document& v_doc)
{
	std::string v_jsonStr;
	bool v_success = false;

	if (File::ReadToStringED(path, v_jsonStr))
	{
		JsonReader::RemoveComments(v_jsonStr);

		try
		{
			simdjson::dom::parser v_parser;
			v_parser.parse_into_document(v_doc, v_jsonStr);

			v_success = true;
		}
	#if defined(_DEBUG) || defined(DEBUG)
		catch (const simdjson::simdjson_error& v_err)
		{
			DebugErrorL("Couldn't parse: ", path, "\nError: ", v_err.what());
		}
	#else
		catch (...) {}
	#endif
	}

	return v_success;
}

bool JsonReader::LoadParseSimdjsonCommentsC(const std::wstring_view& path, simdjson::dom::document& v_doc, const simdjson::dom::element_type& type_check)
{
	std::string v_jsonStr;
	bool v_success = false;

	if (File::ReadToStringED(path, v_jsonStr))
	{
		JsonReader::RemoveComments(v_jsonStr);

		try
		{
			simdjson::dom::parser v_parser;
			const auto v_root = v_parser.parse_into_document(v_doc, v_jsonStr);

			// Yes, this might look bad, but this generates the smallest possible assembly
			if (!v_root.error())
			{
				if (v_root.type() == type_check)
				{
					v_success = true;
				}
				else
				{
					DebugErrorL("Mismatching root json type!\nFile: ", path);
				}
			}
			else
			{
				DebugErrorL("Parse Error:\nFile: ", path, "\nError: ", simdjson::error_message(v_root.error()));
			}
		}
	#if defined(_DEBUG) || defined(DEBUG)
		catch (const simdjson::simdjson_error& v_err)
		{
			DebugErrorL("Couldn't parse: ", path, "\nError: ", v_err.what());
		}
	#else
		catch (...) {}
	#endif
	}

	return v_success;
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
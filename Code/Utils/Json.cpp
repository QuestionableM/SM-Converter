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
	std::string v_output;
	v_output.reserve(json_string.size());

	char* const v_data_beg = json_string.data();
	char* const v_data_end = v_data_beg + json_string.size();

	char* v_data = v_data_beg;

	std::size_t v_curly_bracket_counter = 0;
	std::size_t v_bracket_counter = 0;

	bool v_bracket_flag = false;

	std::size_t v_data_ptr = 0;
	while (v_data != v_data_end)
	{
		switch (*v_data)
		{
		case '{':
		{
			if (!v_bracket_flag) { v_bracket_flag = true; break; }
			v_curly_bracket_counter++;
			break;
		}
		case '[':
		{
			if (!v_bracket_flag) { v_bracket_flag = true; break; }
			v_bracket_counter++;
			break;
		}
		case '}':
		{
			if (v_curly_bracket_counter <= 0)
			{
				v_data++;
				v_output.append(json_string.begin() + v_data_ptr, json_string.begin() + (v_data - v_data_beg));
				json_string = std::move(v_output);
				goto smc_escape_loop_no_append;
			}

			v_curly_bracket_counter--;

			break;
		}
		case ']':
		{
			if (v_bracket_counter <= 0)
			{
				v_data++;
				v_output.append(json_string.begin() + v_data_ptr, json_string.begin() + (v_data - v_data_beg));
				json_string = std::move(v_output);
				goto smc_escape_loop_no_append;
			}

			v_bracket_counter--;
			break;
		}
		case '\"':
		{
			//Find the good string end character
			const char* v_str_end = v_data + 1;
			for (; v_str_end < v_data_end; v_str_end++)
			{
				if (*v_str_end == '\"' && *(v_str_end - 1) != '\\')
					break;
			}

			//Remove the newline characters from json strings to avoid parse errors
			for (; v_data < v_str_end; v_data++)
			{
				if (*v_data == '\n' || *v_data == '\r')
					*v_data = ' ';
			}

			break;
		}
		case '/':
		{
			const char* v_last_char = v_data++;
			if (v_data == v_data_end)
				goto smc_escape_loop;

			switch (*v_data)
			{
			case '/':
			{
				v_output.append(json_string.begin() + v_data_ptr, json_string.begin() + (v_last_char - v_data_beg));

				v_data = strchr(v_data + 1, '\n');
				if (!v_data)
				{
					json_string = std::move(v_output);
					goto smc_escape_loop_no_append;
				}

				v_data_ptr = v_data - v_data_beg;
				continue;
			}
			case '*':
			{
				v_output.append(json_string.begin() + v_data_ptr, json_string.begin() + (v_last_char - v_data_beg));

				v_data = strstr(v_data + 1, "*/");
				if (!v_data)
				{
					json_string = std::move(v_output);
					goto smc_escape_loop_no_append;
				}

				v_data_ptr = (v_data += 2) - v_data_beg;
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

smc_escape_loop:

	if (v_data)
	{
		const std::size_t v_ptr_diff = v_data - v_data_beg;
		const std::size_t v_diff_test = v_ptr_diff - v_data_ptr;
		if (v_diff_test != json_string.size())
		{
			v_output.append(json_string.begin() + v_data_ptr, json_string.begin() + v_ptr_diff);
			json_string = std::move(v_output);
		}
	}

smc_escape_loop_no_append: {}
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
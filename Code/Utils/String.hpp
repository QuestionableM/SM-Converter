#pragma once

#include <sstream>
#include <cwctype>
#include <iomanip>
#include <string>
#include <vector>

#include "Utils\WinInclude.hpp"
#include "Utils\ByteImpl.hpp"

namespace String
{
	inline unsigned char HexStrtolSafe(char* v_ptr)
	{
		char* v_end_ptr;
		const long v_output = strtol(v_ptr, &v_end_ptr, 16);

		if (v_ptr == v_end_ptr)
		{
			*v_ptr = 0;
			return 0;
		}

		*v_ptr = 0;

		return static_cast<unsigned char>(v_output);
	}

	inline std::string ToUtf8(const std::wstring& wstr)
	{
		const int v_count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), NULL, 0, NULL, NULL);

		std::string v_str(v_count, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &v_str[0], v_count, NULL, NULL);

		return v_str;
	}

	inline std::wstring ToWide(const std::string_view& v_str)
	{
		const int v_str_sz = static_cast<int>(v_str.size());
		const int v_count = MultiByteToWideChar(CP_UTF8, 0, v_str.data(), v_str_sz, NULL, 0);

		std::wstring v_wstr(v_count, 0);
		MultiByteToWideChar(CP_UTF8, 0, v_str.data(), v_str_sz, &v_wstr[0], v_count);

		return v_wstr;
	}

	inline std::wstring ToWide(const char* str)
	{
		const int v_str_sz = static_cast<int>(strlen(str));
		const int v_count = MultiByteToWideChar(CP_UTF8, 0, str, v_str_sz, NULL, 0);

		std::wstring v_wstr(v_count, 0);
		MultiByteToWideChar(CP_UTF8, 0, str, v_str_sz, &v_wstr[0], v_count);

		return v_wstr;
	}

	inline std::wstring ToWide(const std::string& str)
	{
		const int v_str_sz = static_cast<int>(str.size());
		const int v_count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), v_str_sz, NULL, 0);

		std::wstring v_wstr(v_count, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), v_str_sz, &v_wstr[0], v_count);

		return v_wstr;
	}

	template<typename T>
	inline void ToLowerR(T& r_str)
	{
		static_assert(std::_Is_any_of_v<T, std::string, std::wstring>, "ToLowerR can only be used with the following types: std::string, std::wstring");

		if constexpr (std::is_same_v<T, std::wstring>) {
			for (wchar_t& w_char : r_str)
				w_char = std::towlower(w_char);
		}
		else if constexpr (std::is_same_v<T, std::string>) {
			for (char& s_char : r_str)
				s_char = std::tolower(s_char);
		}
	}

	template<typename T>
	inline T ToLower(const T& str)
	{
		static_assert(std::_Is_any_of_v<T, std::string, std::wstring>, "ToLower can only be used with the following types: std::string, std::wstring");

		if constexpr (std::is_same_v<T, std::wstring>) {
			std::wstring v_output = str;

			for (wchar_t& w_char : v_output)
				w_char = std::towlower(w_char);

			return v_output;
		}
		else if constexpr (std::is_same_v<T, std::string>) {
			std::string v_output = str;

			for (char& s_char : v_output)
				s_char = std::tolower(s_char);

			return v_output;
		}
	}

	template<typename T>
	inline void ReplaceAllR(T& str, const typename T::value_type& to_replace, const typename T::value_type& replacer)
	{
		static_assert(std::_Is_any_of_v<T, std::string, std::wstring>, "ReplaceAllR can only be used with the following types: std::string, std::wstring");

		std::size_t v_idx = 0;
		while ((v_idx = str.find(to_replace)) != T::npos)
			str[v_idx] = replacer;
	}

	template<typename T>
	inline T ReplaceAll(const T& str, const typename T::value_type& to_replace, const typename T::value_type& replacer)
	{
		static_assert(std::_Is_any_of_v<T, std::string, std::wstring>, "ReplaceAll can only be used with the following types: std::string, std::wstring");

		T str_cpy = str;
		std::size_t v_idx = 0;
		while ((v_idx = str_cpy.find(to_replace)) != T::npos)
			str_cpy[v_idx] = replacer;

		return str_cpy;
	}

	inline std::string BytesToHexString(const std::vector<Byte>& bytes, const int& maxLength, const int& lineLength)
	{
		std::stringstream sstream;
		std::string v_output;
		int a = 1;

		const std::size_t v_loop_sz = std::min(bytes.size(), static_cast<std::size_t>(maxLength));
		for (std::size_t i = 0; i < v_loop_sz; i++)
		{
			sstream << std::setfill('0') << std::setw(sizeof(Byte) * 2) << std::hex << static_cast<int>(bytes[i]);

			v_output.append(sstream.str());
			sstream.str(std::string());

			if ((a++) % lineLength == 0)
				v_output.append("\n\t\t");
		}

		return v_output;
	}

	inline std::string FloatVecToString(const float* f, const std::size_t& amount, const std::string separator = " ")
	{
		std::string v_output;

		v_output.append(std::to_string(f[0]));
		for (std::size_t a = 1; a < amount; a++)
			v_output.append(separator + std::to_string(f[a]));

		return v_output;
	}

	inline bool ReadRegistryKey(const std::wstring& main_key, const std::wstring& sub_key, std::wstring& r_output)
	{
		wchar_t v_data_buf[255] = {};
		DWORD v_buf_sz = sizeof(v_data_buf);

		const LSTATUS v_status = RegGetValueW(
			HKEY_CURRENT_USER,
			main_key.c_str(),
			sub_key.c_str(),
			RRF_RT_REG_SZ,
			NULL,
			reinterpret_cast<PVOID>(v_data_buf),
			&v_buf_sz
		);

		if (v_status == ERROR_SUCCESS)
		{
			r_output = std::wstring(v_data_buf);
			return true;
		}

		return false;
	}

	constexpr const wchar_t g_allowedPathCharacters[] = { L'(', L')', L'.', L' ', L'_', 0x32, L'[', L']', L'-' };
	constexpr const std::size_t g_allowedPathCharactersSz = sizeof(g_allowedPathCharacters) / sizeof(wchar_t);
	inline bool IsPathCharacterAllowed(const wchar_t& c)
	{
		if (std::iswalpha(c) || std::iswdigit(c))
			return true;

		for (std::size_t a = 0; a < g_allowedPathCharactersSz; a++)
			if (g_allowedPathCharacters[a] == c) return true;

		return false;
	}
}
#pragma once

#include "UStd\UnmanagedIomanip.hpp"
#include "UStd\UnmanagedSstream.hpp"
#include "UStd\UnmanagedString.hpp"
#include "UStd\UnmanagedVector.hpp"

#include "Utils\WinInclude.hpp"
#include "Utils\ByteImpl.hpp"

#pragma unmanaged

#include <cwctype>

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
		static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>, "ToLowerR can only be used with the following types: std::string, std::wstring");

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
		static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>, "ToLower can only be used with the following types: std::string, std::wstring");

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
		static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>, "ReplaceAllR can only be used with the following types: std::string, std::wstring");

		std::size_t v_idx = 0;
		while ((v_idx = str.find(to_replace)) != T::npos)
			str[v_idx] = replacer;
	}

	template<typename T>
	inline T ReplaceAll(const T& str, const typename T::value_type& to_replace, const typename T::value_type& replacer)
	{
		static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>, "ReplaceAll can only be used with the following types: std::string, std::wstring");

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

	//Removes the illegal character from string and limits it to a certain size
	template<typename T>
	inline constexpr std::wstring LimitWstring(const T& v_str, const std::size_t& v_char_limit)
	{
		static_assert(std::is_same_v<T, const wchar_t*> || std::is_same_v<T, std::wstring>, "LimitWstring can only work with const wchar_t* and std::wstring");

		if constexpr (std::is_same_v<T, const wchar_t*>)
		{
			const std::size_t v_str_sz = wcslen(v_str);

			std::wstring v_output;
			for (std::size_t a = 0; a < std::min<std::size_t>(v_str_sz, v_char_limit); a++)
			{
				const wchar_t& v_cur_char = v_str[a];

				if (String::IsPathCharacterAllowed(v_cur_char))
					v_output.append(1, v_cur_char);
			}

			return v_output;
		}
		else
		{
			std::wstring v_output;
			for (std::size_t a = 0; a < std::min<std::size_t>(v_str.size(), v_char_limit); a++)
			{
				const wchar_t& v_cur_char = v_str[a];

				if (String::IsPathCharacterAllowed(v_cur_char))
					v_output.append(1, v_cur_char);
			}

			return v_output;
		}
	}

	/*
	 * C++ version 0.4 char* style "itoa":
	 * Written by Lukás Chmela
	 * Released under GPLv3.
	 * This function does not put a null character at the end, so it's up to you to null terminate the string
	*/
	template<typename T, T t_base = 10>
	inline char* FromInteger(T value, char* result)
	{
		// check that the base if valid
		static_assert(t_base >= 2 && t_base <= 36, "Base sould be in range of [2, 36]");
		static_assert(std::is_integral_v<T>, "IntegerToString type must be integral");

		char* ptr = result, * v_end_ptr, tmp_char;
		T tmp_value;

		do {
			tmp_value = value;
			value /= t_base;
			*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * t_base)];
		} while (value);

		if constexpr (t_base == 16 && std::is_same_v<unsigned char, T>)
		{
			if ((ptr - result) == 1)
				*ptr++ = '0';
		}

		if constexpr (!std::is_unsigned_v<T>)
		{
			// Apply negative sign
			if (tmp_value < 0) *ptr++ = '-';
		}

		v_end_ptr = ptr--;

		while (result < ptr) {
			tmp_char = *ptr;
			*ptr-- = *result;
			*result++ = tmp_char;
		}

		return v_end_ptr;
	}

#pragma warning(push)
#pragma warning(disable : 4996)

	//This function does not put a null character at the end, so it's up to you to null terminate the string
	template<std::uint8_t t_precision = 6>
	inline char* FromFloat(float f, char* p)
	{
		typedef union
		{
			int32_t Long;
			float Float;
		} int32_Float_t;

		int32_t mantissa, int_part, frac_part;
		int16_t exp2;
		int32_Float_t x;

		if (f == 0.0)
		{
			*p++ = '0';
			if constexpr (t_precision > (std::uint8_t)0)
			{
				*p++ = '.';
				*p++ = '0';
			}

			return p;
		}

		x.Float = f;
		exp2 = (unsigned char)(x.Long >> 23) - 127;
		mantissa = (x.Long & 0xFFFFFF) | 0x800000;
		frac_part = 0;
		int_part = 0;

		if (exp2 >= 31)
		{
			*p++ = '2';
			*p++ = '1';
			*p++ = '4';
			*p++ = '7';
			*p++ = '4';
			*p++ = '8';
			*p++ = '3';
			*p++ = '5';
			*p++ = '2';
			*p++ = '0';

			return p;

		}
		else if (exp2 < -23)
		{
			*p++ = '0';

			return p;
		}
		else if (exp2 >= 23)
			int_part = mantissa << (exp2 - 23);
		else if (exp2 >= 0)
		{
			int_part = mantissa >> (23 - exp2);
			frac_part = (mantissa << (exp2 + 1)) & 0xFFFFFF;
		}
		else
		{
			//if (exp2 < 0)
			frac_part = (mantissa & 0xFFFFFF) >> -(exp2 + 1);
		}

		if (x.Long < 0)
		{
			*p++ = '-';
		}

		if (int_part == 0)
			*p++ = '0';
		else
		{
			_ltoa(int_part, p, 10);
			while (*p) p++;
		}

		if constexpr (t_precision > (std::uint8_t)0)
		{
			*p++ = '.';
			if (frac_part == 0)
			{
				*p++ = '0';
			}
			else
			{
				char m;

				for (m = 0; m < t_precision; m++)
				{
					//frac_part *= 10;
					frac_part = (frac_part << 3) + (frac_part << 1);
					*p++ = (frac_part >> 24) + '0';
					frac_part &= 0xFFFFFF;
				}

				//delete ending zeroes
				for (--p; p[0] == '0' && p[-1] != '.'; --p) {}
				++p;
			}
		}

		return p;
	}

#pragma warning(pop)
}

#pragma managed
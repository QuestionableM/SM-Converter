#pragma once

#include "UStd/UnmanagedIomanip.hpp"
#include "UStd/UnmanagedSstream.hpp"
#include "UStd/UnmanagedString.hpp"
#include "UStd/UnmanagedVector.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/WinInclude.hpp"
#include "Utils/ByteImpl.hpp"

SM_UNMANAGED_CODE

#include <cwctype>

namespace String
{
	struct HexAsciiToByte
	{
		inline static constexpr std::uint8_t GetValue(std::size_t val)
		{
			if (val >= std::size_t('0') && val <= std::size_t('9'))
				return std::uint8_t(val - '0');
			else if (val >= std::size_t('A') && val <= std::size_t('F'))
				return std::uint8_t(val - 'A' + 10);
			else if (val >= std::size_t('a') && val <= std::size_t('f'))
				return std::uint8_t(val - 'a' + 10);
			else
				return 0;
		}

		inline constexpr HexAsciiToByte() : m_table()
		{
			for (std::size_t a = 0; a < sizeof(m_table); a++)
				m_table[a] = GetValue(a);
		}

		inline std::uint8_t operator[](std::size_t idx) const noexcept
		{
			return m_table[idx];
		}

		std::uint8_t m_table[0xff];
	};

	inline constexpr static HexAsciiToByte g_hexLookupTable{};

	// Reads 2 characters from the byte_pair and converts them into a byte
	inline std::uint8_t HexStringToByte(const char* byte_pair) noexcept
	{
		const std::uint8_t v_partOne = g_hexLookupTable[byte_pair[0]];
		const std::uint8_t v_partTwo = g_hexLookupTable[byte_pair[1]];

		return v_partTwo | v_partOne << 4;
	}

	constexpr static const char g_byteToCharLookupTable[] =
	{
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
	};

	inline char* ByteToHexString(char* bufferIn, std::uint8_t val) noexcept
	{
		*bufferIn++ = g_byteToCharLookupTable[(val >> 4) & 0b1111];
		*bufferIn++ = g_byteToCharLookupTable[val & 0b1111];
		return bufferIn;
	}

	inline unsigned char HexStrtolSafe(char* v_ptr)
	{
		char* v_end_ptr;
		const long v_output = strtol(v_ptr, &v_end_ptr, 16);

		*v_ptr = 0;
		if (v_ptr == v_end_ptr) return 0;

		return static_cast<unsigned char>(v_output);
	}

	template<typename T>
	inline char* IntegerToBuffer(char* buffer, std::make_unsigned_t<T> num)
	{
		do {
			*--buffer = char(num % 10) + '0';
			num /= 10;
		} while (num != 0);

		return buffer;
	}

	template<typename T> requires(std::is_signed_v<T>)
	inline std::string_view IntegerToStringS(char* buffer, std::size_t buffer_sz, std::make_signed_t<T> num)
	{
		char* v_bufferPtr = buffer + buffer_sz;

		if (num < 0)
		{
			v_bufferPtr = String::IntegerToBuffer(v_bufferPtr, static_cast<std::make_unsigned_t<T>>(0 - num));
			*--v_bufferPtr = '-';
		}
		else
		{
			v_bufferPtr = String::IntegerToBuffer(v_bufferPtr, num);
		}

		return std::string_view(v_bufferPtr, buffer + buffer_sz);
	}

	template<typename T> requires(std::is_unsigned_v<T>)
	inline std::string_view IntegerToStringU(char* buffer, std::size_t buffer_sz, std::make_unsigned_t<T> num)
	{
		return std::string_view(
			String::IntegerToBuffer<T>(buffer + buffer_sz, num),
			buffer + buffer_sz
		);
	}

	template<typename T>
	inline std::string_view IntegerToString(char* buffer, std::size_t buffer_sz, T num)
	{
		if constexpr (std::is_unsigned_v<T>) {
			return String::IntegerToStringU<T>(buffer, buffer_sz, num);
		} else {
			return String::IntegerToStringS<T>(buffer, buffer_sz, num);
		}
	}

	template<typename T>
	inline void AppendIntegerToString(std::string& outStr, T num)
	{
		char v_intBuffer[64];
		outStr.append(String::IntegerToString<T>(v_intBuffer, sizeof(v_intBuffer), num));
	}

	inline std::string ToUtf8(const std::wstring_view& wstr)
	{
		const int v_count = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), NULL, 0, NULL, NULL);

		std::string v_str(v_count, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr.data(), -1, &v_str[0], v_count, NULL, NULL);

		return v_str;
	}

	inline void ToWideRef(const std::string_view& str, std::wstring& outStr)
	{
		const int v_strSz = static_cast<int>(str.size());
		const int v_count = MultiByteToWideChar(CP_UTF8, 0, str.data(), v_strSz, NULL, 0);

		outStr.resize(v_count);
		MultiByteToWideChar(CP_UTF8, 0, str.data(), v_strSz, outStr.data(), v_count);
	}

	inline std::wstring ToWide(const std::string_view& v_str)
	{
		const int v_str_sz = static_cast<int>(v_str.size());
		const int v_count = MultiByteToWideChar(CP_UTF8, 0, v_str.data(), v_str_sz, NULL, 0);

		std::wstring v_wstr(v_count, 0);
		MultiByteToWideChar(CP_UTF8, 0, v_str.data(), v_str_sz, &v_wstr[0], v_count);

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
	inline auto ToLower(const T& str)
	{
		static_assert(
			std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view> ||
			std::is_same_v<T, std::wstring> || std::is_same_v<T, std::wstring_view>,
			"ToLower can only be used with the following types: std::string, std::string_view, std::wstring, std::wstring_view");

		using TStringType = std::conditional_t<
			std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>,
			std::string,
			std::wstring
		>;

		TStringType v_output(str);
		String::ToLowerR(v_output);

		return v_output;
	}

	template<typename T>
	inline void ReplaceAllR(T& str, const typename T::value_type& to_replace, const typename T::value_type& replacer)
	{
		static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>, "ReplaceAllR can only be used with the following types: std::string, std::wstring");

		typename T::value_type* v_iter = str.data();

		if constexpr (std::is_same_v<T, std::string>) {
			while ((v_iter = strchr(v_iter, to_replace)) != nullptr)
				*v_iter = replacer;
		} else if constexpr (std::is_same_v<T, std::wstring>) {
			while ((v_iter = wcschr(v_iter, to_replace)) != nullptr)
				*v_iter = replacer;
		}
	}

	template<typename T>
	inline T ReplaceAll(const T& str, const typename T::value_type& to_replace, const typename T::value_type& replacer)
	{
		static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>, "ReplaceAll can only be used with the following types: std::string, std::wstring");

		T str_cpy = str;
		String::ReplaceAllR(str_cpy, to_replace, replacer);

		return str_cpy;
	}

	inline std::string BytesToHexString(const std::vector<Byte>& bytes, int maxLength, int lineLength)
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

	inline std::string FloatVecToString(const float* f, std::size_t amount, const std::string& separator = " ")
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

	//Spaces cause a file write error, so we have to make sure that there are no spaces
	inline void RemoveSpacesFromEndR(std::wstring& v_wstr)
	{
		while (v_wstr.back() == L' ')
			v_wstr.pop_back();
	}

	constexpr const wchar_t g_allowedPathCharacters[] = { L'(', L')', L'[', L']', L'-', L'_', L' ', 0x32 };
	constexpr const std::size_t g_allowedPathCharactersSz = sizeof(g_allowedPathCharacters) / sizeof(wchar_t);
	inline bool IsPathCharacterAllowed(wchar_t c)
	{
		if (std::iswalpha(c) || std::iswdigit(c))
			return true;

		for (std::size_t a = 0; a < g_allowedPathCharactersSz; a++)
			if (g_allowedPathCharacters[a] == c) return true;

		return false;
	}

	//Removes the illegal character from string and limits it to a certain size
	template<typename T>
	inline constexpr std::wstring LimitWstring(const T& v_str, std::size_t v_char_limit)
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
	 * Written by Lukas Chmela
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

	/*
	 * C++ version 0.4 char* style "itoa":
	 * Written by Lukas Chmela
	 * Released under GPLv3.
	 * This function does not put a null character at the end, so it's up to you to null terminate the string
	*/
	template<typename T, T t_base = 10>
	inline wchar_t* FromIntegerW(T value, wchar_t* result)
	{
		// check that the base if valid
		static_assert(t_base >= 2 && t_base <= 36, "Base sould be in range of [2, 36]");
		static_assert(std::is_integral_v<T>, "IntegerToString type must be integral");

		wchar_t* ptr = result, * v_end_ptr, tmp_char;
		T tmp_value;

		do {
			tmp_value = value;
			value /= t_base;
			*ptr++ = L"zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * t_base)];
		} while (value);

		if constexpr (t_base == 16 && std::is_same_v<unsigned char, T>)
		{
			if ((ptr - result) == 1)
				*ptr++ = L'0';
		}

		if constexpr (!std::is_unsigned_v<T>)
		{
			// Apply negative sign
			if (tmp_value < 0) *ptr++ = L'-';
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

SM_MANAGED_CODE
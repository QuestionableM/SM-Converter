#pragma once

#include "UStd/UnmanagedString.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/ByteImpl.hpp"
#include "Utils/String.hpp"

SM_UNMANAGED_CODE

class SMColor
{
public:
	inline SMColor() noexcept : m_color(0x000000ff) {}

	inline SMColor(const std::string_view& color) { this->fromStringView(color); }

	inline SMColor(Byte r, Byte g, Byte b) :
		m_bytes{ r, g, b, 255 }
	{}

	inline SMColor(Byte r_val, Byte g_val, Byte b_val, Byte a_val) :
		r(r_val),
		g(g_val),
		b(b_val),
		a(a_val)
	{}

	inline SMColor(unsigned int color)
	{
		this->SetIntLittleEndian(color);
	}

	inline void operator=(const std::string_view& color) { this->fromStringView(color); }

	inline void SetIntLittleEndian(unsigned int color)
	{
		this->r = static_cast<Byte>(color);
		this->g = static_cast<Byte>(color >> 8);
		this->b = static_cast<Byte>(color >> 16);
		this->a = static_cast<Byte>(color >> 24);
	}

	inline void SetIntBigEndian(unsigned int color)
	{
		this->r = static_cast<Byte>(color >> 24);
		this->g = static_cast<Byte>(color >> 16);
		this->b = static_cast<Byte>(color >> 8);
		this->a = static_cast<Byte>(color);
	}

	template<class T>
	constexpr inline void SetRGBFloat(T r, T g, T b)
	{
		static_assert(std::is_floating_point_v<T>, "SetRGBFloat can only be used with floating point types");

		this->r = static_cast<Byte>(std::max(static_cast<T>(0.0), std::min(r, static_cast<T>(1.0))) * static_cast<T>(255.0));
		this->g = static_cast<Byte>(std::max(static_cast<T>(0.0), std::min(g, static_cast<T>(1.0))) * static_cast<T>(255.0));
		this->b = static_cast<Byte>(std::max(static_cast<T>(0.0), std::min(b, static_cast<T>(1.0))) * static_cast<T>(255.0));
	}

	template<class T>
	constexpr inline void SetRGBAFloat(T r, T g, T b, T a)
	{
		static_assert(std::is_floating_point_v<T>, "SetRGBAFloat can only be used with floating point types");

		this->r = static_cast<Byte>(std::max(static_cast<T>(0.0), std::min(r, static_cast<T>(1.0))) * static_cast<T>(255.0));
		this->g = static_cast<Byte>(std::max(static_cast<T>(0.0), std::min(g, static_cast<T>(1.0))) * static_cast<T>(255.0));
		this->b = static_cast<Byte>(std::max(static_cast<T>(0.0), std::min(b, static_cast<T>(1.0))) * static_cast<T>(255.0));
		this->a = static_cast<Byte>(std::max(static_cast<T>(0.0), std::min(a, static_cast<T>(1.0))) * static_cast<T>(255.0));
	}

	inline std::string String() const
	{
		return std::to_string(this->r) + " " + std::to_string(this->g) + " " + std::to_string(this->b);
	}

	inline std::string StringNormalized() const
	{
		const float v_norm_r = static_cast<float>(this->r) / 255.0f;
		const float v_norm_g = static_cast<float>(this->g) / 255.0f;
		const float v_norm_b = static_cast<float>(this->b) / 255.0f;

		return std::to_string(v_norm_r) + " " + std::to_string(v_norm_g) + " " + std::to_string(v_norm_b);
	}

	inline static char* WriteEmptyHex(char* v_ptr)
	{
		*v_ptr++ = '0';
		*v_ptr++ = '0';
		*v_ptr++ = '0';
		*v_ptr++ = '0';
		*v_ptr++ = '0';
		*v_ptr++ = '0';

		return v_ptr;
	}

	//Returns the end of the hex string
	inline char* StringHexCStr(char* v_ptr) const
	{
		v_ptr = String::FromInteger<unsigned char, 16>(this->r, v_ptr);
		v_ptr = String::FromInteger<unsigned char, 16>(this->g, v_ptr);
		return String::FromInteger<unsigned char, 16>(this->b, v_ptr);
	}

	void appendStringHex(std::string& outStr) const
	{
		char v_buffer[6], *v_ptr;
		v_ptr = String::FromInteger<std::uint8_t, 16>(this->r, v_buffer);
		v_ptr = String::FromInteger<std::uint8_t, 16>(this->g, v_ptr);
		String::FromInteger<std::uint8_t, 16>(this->b, v_ptr);

		outStr.append(v_buffer, 6);
	}

	inline std::string StringHex() const
	{
		char v_buffer[7], *v_ptr;

		v_ptr = String::FromInteger<unsigned char, 16>(this->r, v_buffer);
		v_ptr = String::FromInteger<unsigned char, 16>(this->g, v_ptr);
		String::FromInteger<unsigned char, 16>(this->b, v_ptr);

		return std::string(v_buffer, 6);
	}

	//Convert to string with alpha component
	inline std::string StringHexAlpha() const
	{
		char v_buffer[9], *v_ptr;

		v_ptr = String::FromInteger<unsigned char, 16>(this->r, v_buffer);
		v_ptr = String::FromInteger<unsigned char, 16>(this->g, v_ptr);
		v_ptr = String::FromInteger<unsigned char, 16>(this->b, v_ptr);
		String::FromInteger<unsigned char, 16>(this->a, v_ptr);

		return std::string(v_buffer, 8);
	}

	template<typename T>
	inline T GetFloat(std::size_t idx) const
	{
		static_assert(std::is_floating_point_v<T>, "GetFloat can only be used with floating point types");

		return static_cast<T>(m_bytes[idx]) / static_cast<T>(255.0);
	}

	template<typename T>
	constexpr inline void SetFloat(std::size_t idx, T fp_val)
	{
		static_assert(std::is_floating_point_v<T>, "SetFloat can only be used with floating point types");

		m_bytes[idx] = static_cast<Byte>(std::max(static_cast<T>(0.0), std::min(fp_val, static_cast<T>(1.0))) * static_cast<T>(255.0));
	}

	inline void fromStringView(const std::string_view& color)
	{
		if (color.empty())
		{
			m_color = 0x000000ff;
			return;
		}

		const std::size_t v_ptrAdder(color[0] == '#');
		const std::size_t v_lessCheck = 6 + v_ptrAdder;
		const std::size_t v_moreCheck = 8 + v_ptrAdder;

		if (color.size() < v_lessCheck)
		{
			m_color = 0x000000ff;
			return;
		}

		char v_strCpy[9] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 'f', 'f', 0x0 };
		std::memcpy(v_strCpy, color.data() + v_ptrAdder, (color.size() >= v_moreCheck) ? 8 : 6);

		this->r = String::HexStringToByte(v_strCpy);
		this->g = String::HexStringToByte(v_strCpy + 2);
		this->b = String::HexStringToByte(v_strCpy + 4);
		this->a = String::HexStringToByte(v_strCpy + 6);
	}

	union
	{
		Byte m_bytes[4];
		unsigned int m_color;

		struct { Byte r, g, b, a; };
	};
};

SM_MANAGED_CODE
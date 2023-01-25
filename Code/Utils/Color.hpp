#pragma once

#include "UStd\UnmanagedString.hpp"

#include "Utils\ByteImpl.hpp"
#include "Utils\String.hpp"

#pragma unmanaged

class SMColor
{
public:
	inline SMColor() { m_color = 0x000000ff; }

	inline SMColor(const std::string& color) { this->FromString<std::string>(color); }
	inline SMColor(const std::wstring& color) { this->FromString<std::wstring>(color); }
	inline SMColor(const char* color) { this->FromCString(color); }

	inline SMColor(const Byte& r, const Byte& g, const Byte& b)
	{
		m_bytes[0] = r;
		m_bytes[1] = g;
		m_bytes[2] = b;
		m_bytes[3] = 255;
	}

	inline SMColor(const Byte& r, const Byte& g, const Byte& b, const Byte& a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	inline SMColor(const unsigned int& color)
	{
		this->SetIntLittleEndian(color);
	}


	inline void operator=(const std::string& color) { this->FromString<std::string>(color); }
	inline void operator=(const std::wstring& color) { this->FromString<std::wstring>(color); }
	inline void operator=(const char* color) { this->FromCString(color); }

	inline void SetIntLittleEndian(const unsigned int& color)
	{
		this->r = static_cast<Byte>(color);
		this->g = static_cast<Byte>(color >> 8);
		this->b = static_cast<Byte>(color >> 16);
		this->a = static_cast<Byte>(color >> 24);
	}

	inline void SetIntBigEndian(const unsigned int& color)
	{
		this->r = static_cast<Byte>(color >> 24);
		this->g = static_cast<Byte>(color >> 16);
		this->b = static_cast<Byte>(color >> 8);
		this->a = static_cast<Byte>(color);
	}

	template<class T>
	constexpr inline void SetRGBFloat(const T& r, const T& g, const T& b)
	{
		static_assert(std::is_floating_point_v<T>, "SetRGBFloat can only be used with floating point types");

		this->r = static_cast<Byte>(std::max(static_cast<T>(0.0), std::min(r, static_cast<T>(1.0))) * static_cast<T>(255.0));
		this->g = static_cast<Byte>(std::max(static_cast<T>(0.0), std::min(g, static_cast<T>(1.0))) * static_cast<T>(255.0));
		this->b = static_cast<Byte>(std::max(static_cast<T>(0.0), std::min(b, static_cast<T>(1.0))) * static_cast<T>(255.0));
	}

	template<class T>
	constexpr inline void SetRGBAFloat(const T& r, const T& g, const T& b, const T& a)
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
	inline T GetFloat(const std::size_t& idx) const
	{
		static_assert(std::is_floating_point_v<T>, "GetFloat can only be used with floating point types");

		return static_cast<T>(m_bytes[idx]) / static_cast<T>(255.0);
	}

	template<typename T>
	constexpr inline void SetFloat(const std::size_t& idx, const T& fp_val)
	{
		static_assert(std::is_floating_point_v<T>, "SetFloat can only be used with floating point types");

		m_bytes[idx] = static_cast<Byte>(std::max(static_cast<T>(0.0), std::min(fp_val, static_cast<T>(1.0))) * static_cast<T>(255.0));
	}

	inline void FromCString(const char* color)
	{
		const std::size_t v_str_len = strlen(color);
		if (v_str_len == 0)
		{
			m_color = 0x000000ff;
			return;
		}

		char v_str_cpy[9] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 'f', 'f', 0x0};

		if (color[0] == '#')
		{
			if (v_str_len < 7)
			{
				m_color = 0x000000ff;
				return;
			}
			
			std::memcpy(v_str_cpy, color + 1, (v_str_len >= 9) ? 8 : 6);
		}
		else
		{
			if (v_str_len < 6)
			{
				m_color = 0x000000ff;
				return;
			}

			std::memcpy(v_str_cpy, color, (v_str_len >= 8) ? 8 : 6);
		}

		this->a = String::HexStrtolSafe(v_str_cpy + 6);
		this->b = String::HexStrtolSafe(v_str_cpy + 4);
		this->g = String::HexStrtolSafe(v_str_cpy + 2);
		this->r = String::HexStrtolSafe(v_str_cpy);
	}

	template<typename T>
	inline void FromString(const T& color)
	{
		static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>, "Color::FromString can only be used with std::string, std::wstring");

		if (color.empty())
		{
			m_color = 0x000000ff;
			return;
		}

		if (color[0] == '#')
		{
			if (color.size() < 7)
			{
				m_color = 0x000000ff;
				return;
			}

			this->r = static_cast<Byte>(std::stoi(color.substr(1, 2), nullptr, 16));
			this->g = static_cast<Byte>(std::stoi(color.substr(3, 2), nullptr, 16));
			this->b = static_cast<Byte>(std::stoi(color.substr(5, 2), nullptr, 16));

			if (color.size() >= 9)
				this->a = static_cast<Byte>(std::stoi(color.substr(7, 2), nullptr, 16));
			else
				this->a = 255;
		}
		else
		{
			if (color.size() < 6)
			{
				m_color = 0x000000ff;
				return;
			}

			this->r = static_cast<Byte>(std::stoi(color.substr(0, 2), nullptr, 16));
			this->g = static_cast<Byte>(std::stoi(color.substr(2, 2), nullptr, 16));
			this->b = static_cast<Byte>(std::stoi(color.substr(4, 2), nullptr, 16));

			if (color.size() >= 8)
				this->a = static_cast<Byte>(std::stoi(color.substr(6, 2), nullptr, 16));
			else
				this->a = 255;
		}
	}

	union
	{
		Byte m_bytes[4];
		unsigned int m_color;

		struct
		{
			Byte r;
			Byte g;
			Byte b;
			Byte a;
		};
	};
};

#pragma managed
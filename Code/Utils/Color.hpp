#pragma once

#include "UStd/UnmanagedString.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/ByteImpl.hpp"
#include "Utils/String.hpp"

SM_UNMANAGED_CODE

class SMColor
{
public:
	SMColor() noexcept;
	SMColor(const std::string_view& color) noexcept;

	SMColor(Byte r, Byte g, Byte b) noexcept;
	SMColor(Byte r_val, Byte g_val, Byte b_val, Byte a_val) noexcept;
	SMColor(std::uint32_t color) noexcept;

	void operator=(const std::string_view& color) noexcept;

	void SetIntLittleEndian(std::uint32_t color) noexcept;
	void SetIntBigEndian(std::uint32_t color) noexcept;

	std::string String() const;
	std::string StringNormalized() const;
	std::string StringHex() const;
	std::string StringHexAlpha() const;
	//Returns the end of the hex string
	char* StringHexCStr(char* ptr) const noexcept;
	void appendStringHex(std::string& outStr) const;
	static char* WriteEmptyHex(char* ptr) noexcept;

	inline void fromStringView(const std::string_view& color) noexcept;

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

	union
	{
		Byte m_bytes[4];
		unsigned int m_color;

		struct { Byte r, g, b, a; };
	};
};

SM_MANAGED_CODE
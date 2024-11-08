#pragma once

#include "UStd/UnmanagedString.hpp"
#include "Utils/clr_include.hpp"

SM_MANAGED_CODE

namespace Hashing
{
	struct PrecomputedStringHash
	{
		inline PrecomputedStringHash(const std::string_view& str) noexcept :
			m_str(str),
			m_hash(std::hash<std::string_view>{}(str))
		{}

		std::string_view m_str;
		std::size_t m_hash;
	};

	inline bool operator==(PrecomputedStringHash lhs, const std::string& rhs) noexcept
	{
		return lhs.m_str == std::string_view(rhs);
	}

	inline bool operator==(PrecomputedStringHash lhs, const std::string_view& rhs) noexcept
	{
		return lhs.m_str == rhs;
	}

	struct StringHasher
	{
		using is_transparent = void;

		inline std::size_t operator()(const std::string& str) const noexcept
		{
			return std::hash<std::string>{}(str);
		}

		inline std::size_t operator()(const std::string_view& str) const noexcept
		{
			return std::hash<std::string_view>{}(str);
		}

		inline std::size_t operator()(PrecomputedStringHash hash) const noexcept
		{
			return hash.m_hash;
		}
	};
}

SM_UNMANAGED_CODE
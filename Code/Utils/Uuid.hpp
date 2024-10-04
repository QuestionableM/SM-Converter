#pragma once

#include "UStd/UnmanagedString.hpp"
#include "UStd/UnmanagedVector.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/WinInclude.hpp"
#include "Utils/Console.hpp"
#include "Utils/String.hpp"
#include "Utils/Crypt.hpp"

SM_UNMANAGED_CODE

class SMUuid
{
public:
	static const SMUuid Null;

	SMUuid();
	SMUuid(const std::string_view& uuid);
	SMUuid(const std::vector<long long>& longs);
	SMUuid(long long first, long long second, bool isBigEndian = false);

	bool IsNil() const noexcept;

	static SMUuid GenerateNamedStatic(const SMUuid& uuid, const std::string& some_string);
	void GenerateNamed(const SMUuid& uuid, const std::string& str);

	std::size_t Hash() const;

	//Returns the end of the uuid
	char* ToCString(char* v_beginning) const;
	wchar_t* ToCStringW(wchar_t* v_beginning) const;

	std::string ToString() const;
	std::wstring ToWstring() const;

	inline void fromStringView(const std::string_view& uuid)
	{
		if (uuid.size() != 36)
		{
			std::memset(m_Data8, 0, sizeof(m_Data8));
			return;
		}

		const constexpr static std::size_t v_offsets[16] = { 0, 2, 4, 6, 9, 11, 14, 16, 19, 21, 24, 26, 28, 30, 32, 34 };
		for (std::size_t a = 0; a < sizeof(m_Data8); a++)
			m_Data8[a] = String::HexStringToByte(uuid.data() + v_offsets[a]);
	}

public:
	friend bool operator==(const SMUuid& lhs, const SMUuid& rhs) noexcept;
	friend bool operator!=(const SMUuid& lhs, const SMUuid& rhs) noexcept;
	friend bool operator< (const SMUuid& lhs, const SMUuid& rhs) noexcept;

	union
	{
		unsigned __int8  m_Data8[16];
		unsigned __int16 m_Data16[8];
		unsigned __int32 m_Data32[4];
		unsigned __int64 m_Data64[2];
	};
};

inline bool operator==(const SMUuid& lhs, const SMUuid& rhs) noexcept
{
	return (
		lhs.m_Data64[0] == rhs.m_Data64[0] &&
		lhs.m_Data64[1] == rhs.m_Data64[1]
	);
}

inline bool operator!=(const SMUuid& lhs, const SMUuid& rhs) noexcept
{
	return (
		lhs.m_Data64[0] != rhs.m_Data64[0] ||
		lhs.m_Data64[1] != rhs.m_Data64[1]
	);
}

inline bool operator< (const SMUuid& lhs, const SMUuid& rhs) noexcept
{
	return (
		lhs.m_Data64[0] < rhs.m_Data64[0] ||
		lhs.m_Data64[1] < rhs.m_Data64[1]
	);
}

namespace std
{
	template<>
	struct hash<SMUuid>
	{
		using argument_type = SMUuid;
		using result_type = std::size_t;

		inline result_type operator()(argument_type const& uuid) const
		{
			return uuid.Hash();
		}
	};
}

SM_MANAGED_CODE
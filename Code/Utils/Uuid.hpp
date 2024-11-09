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


	static SMUuid GenerateNamedStatic(const SMUuid& uuid, const std::string& some_string);
	void GenerateNamed(const SMUuid& uuid, const std::string& str);

	std::size_t hash() const noexcept;
	bool isNil() const noexcept;

	//Returns the end of the uuid
	char* toCString(char* buff) const;
	wchar_t* toCStringW(wchar_t* buff) const;

	std::string toString() const;
	std::wstring toWstring() const;

	void fromStringView(const std::string_view& uuid);

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

		inline result_type operator()(argument_type const& uuid) const noexcept
		{
			return uuid.hash();
		}
	};
}

struct PrecomputedUuidHash
{
	inline PrecomputedUuidHash(const SMUuid& uuid) noexcept :
		m_uuid(uuid),
		m_hash(uuid.hash())
	{}

	SMUuid m_uuid;
	std::size_t m_hash;
};

inline bool operator==(const SMUuid& lhs, PrecomputedUuidHash rhs) noexcept
{
	return lhs == rhs.m_uuid;
}

struct UuidHasher
{
	using is_transparent = void;

	inline std::size_t operator()(const SMUuid& uuid) const noexcept
	{
		return std::hash<SMUuid>{}(uuid);
	}

	inline std::size_t operator()(PrecomputedUuidHash hash) const noexcept
	{
		return hash.m_hash;
	}
};

SM_MANAGED_CODE
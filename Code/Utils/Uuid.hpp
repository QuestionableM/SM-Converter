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
	SMUuid(const std::string& uuid);
	SMUuid(const std::wstring& uuid);
	SMUuid(const char* uuid);
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

	void FromCString(const char* v_str);

	template<typename T>
	inline void FromString(const T& uuid)
	{
		static_assert(std::_Is_any_of_v<T, std::string, std::wstring>, "Uuid::FromString can only be used for the following types: std::string, std::wstring");

		if (uuid.size() != 36)
		{
			std::memset(m_Data8, 0, 16);
			return;
		}

		m_Data8[0] = static_cast<unsigned char>(std::stoi(uuid.substr(0, 2), nullptr, 16));
		m_Data8[1] = static_cast<unsigned char>(std::stoi(uuid.substr(2, 2), nullptr, 16));
		m_Data8[2] = static_cast<unsigned char>(std::stoi(uuid.substr(4, 2), nullptr, 16));
		m_Data8[3] = static_cast<unsigned char>(std::stoi(uuid.substr(6, 2), nullptr, 16));

		m_Data8[4] = static_cast<unsigned char>(std::stoi(uuid.substr(9, 2), nullptr, 16));
		m_Data8[5] = static_cast<unsigned char>(std::stoi(uuid.substr(11, 2), nullptr, 16));

		m_Data8[6] = static_cast<unsigned char>(std::stoi(uuid.substr(14, 2), nullptr, 16));
		m_Data8[7] = static_cast<unsigned char>(std::stoi(uuid.substr(16, 2), nullptr, 16));

		m_Data8[8] = static_cast<unsigned char>(std::stoi(uuid.substr(19, 2), nullptr, 16));
		m_Data8[9] = static_cast<unsigned char>(std::stoi(uuid.substr(21, 2), nullptr, 16));

		m_Data8[10] = static_cast<unsigned char>(std::stoi(uuid.substr(24, 2), nullptr, 16));
		m_Data8[11] = static_cast<unsigned char>(std::stoi(uuid.substr(26, 2), nullptr, 16));
		m_Data8[12] = static_cast<unsigned char>(std::stoi(uuid.substr(28, 2), nullptr, 16));
		m_Data8[13] = static_cast<unsigned char>(std::stoi(uuid.substr(30, 2), nullptr, 16));
		m_Data8[14] = static_cast<unsigned char>(std::stoi(uuid.substr(32, 2), nullptr, 16));
		m_Data8[15] = static_cast<unsigned char>(std::stoi(uuid.substr(34, 2), nullptr, 16));
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
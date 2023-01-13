#pragma once

#include "Utils\WinInclude.hpp"
#include "Utils\String.hpp"
#include "Utils\Crypt.hpp"

#include <iomanip>
#include <sstream>
#include <vector>
#include <string>

class SMUuid
{
public:
	inline SMUuid() { std::memset(m_Data8, 0, 16); }

	inline SMUuid(const std::string& uuid) { this->FromString<std::string>(uuid); }
	inline SMUuid(const std::wstring& uuid) { this->FromString<std::wstring>(uuid); }
	inline SMUuid(const char* uuid) { this->FromCString(uuid); }

	inline SMUuid(const std::vector<long long>& longs)
	{
		std::memcpy(&m_Data64, longs.data(), 16);
	}

	inline SMUuid(const long long& first, const long long& second, const bool& isBigEndian = false)
	{
		m_Data64[0] = first;
		m_Data64[1] = second;

		if (isBigEndian)
		{
			std::reverse(m_Data8, m_Data8 + 16);
		}
	}

	inline static SMUuid GenerateNamedStatic(const SMUuid& uuid, const std::string& some_string)
	{
		const std::string v_hash_string = uuid.ToString() + some_string;
		const std::string v_hash = Crypt::GetHashString(v_hash_string.data(), v_hash_string.size(), Crypt::HashType::Sha1);

		SMUuid v_new_uuid;

		v_new_uuid.m_Data8[0] = static_cast<unsigned char>(std::stoi(v_hash.substr(0, 2), nullptr, 16));
		v_new_uuid.m_Data8[1] = static_cast<unsigned char>(std::stoi(v_hash.substr(2, 2), nullptr, 16));
		v_new_uuid.m_Data8[2] = static_cast<unsigned char>(std::stoi(v_hash.substr(4, 2), nullptr, 16));
		v_new_uuid.m_Data8[3] = static_cast<unsigned char>(std::stoi(v_hash.substr(6, 2), nullptr, 16));
		v_new_uuid.m_Data8[4] = static_cast<unsigned char>(std::stoi(v_hash.substr(8, 2), nullptr, 16));
		v_new_uuid.m_Data8[5] = static_cast<unsigned char>(std::stoi(v_hash.substr(10, 2), nullptr, 16));
		v_new_uuid.m_Data8[6] = static_cast<unsigned char>(std::stoi(v_hash.substr(12, 2), nullptr, 16));
		v_new_uuid.m_Data8[7] = static_cast<unsigned char>(std::stoi(v_hash.substr(14, 2), nullptr, 16));
		v_new_uuid.m_Data8[8] = static_cast<unsigned char>(std::stoi(v_hash.substr(16, 2), nullptr, 16));
		v_new_uuid.m_Data8[9] = static_cast<unsigned char>(std::stoi(v_hash.substr(18, 2), nullptr, 16));
		v_new_uuid.m_Data8[10] = static_cast<unsigned char>(std::stoi(v_hash.substr(20, 2), nullptr, 16));
		v_new_uuid.m_Data8[11] = static_cast<unsigned char>(std::stoi(v_hash.substr(22, 2), nullptr, 16));
		v_new_uuid.m_Data8[12] = static_cast<unsigned char>(std::stoi(v_hash.substr(24, 2), nullptr, 16));
		v_new_uuid.m_Data8[13] = static_cast<unsigned char>(std::stoi(v_hash.substr(26, 2), nullptr, 16));
		v_new_uuid.m_Data8[14] = static_cast<unsigned char>(std::stoi(v_hash.substr(28, 2), nullptr, 16));
		v_new_uuid.m_Data8[15] = static_cast<unsigned char>(std::stoi(v_hash.substr(30, 2), nullptr, 16));

		return v_new_uuid;
	}

	inline void GenerateNamed(const SMUuid& uuid, const std::string& str)
	{
		const std::string v_hash_str = uuid.ToString() + str;
		const std::string v_hash = Crypt::GetHashString(v_hash_str.data(), v_hash_str.size(), Crypt::HashType::Sha1);

		m_Data8[0] = static_cast<unsigned char>(std::stoi(v_hash.substr(0, 2), nullptr, 16));
		m_Data8[1] = static_cast<unsigned char>(std::stoi(v_hash.substr(2, 2), nullptr, 16));
		m_Data8[2] = static_cast<unsigned char>(std::stoi(v_hash.substr(4, 2), nullptr, 16));
		m_Data8[3] = static_cast<unsigned char>(std::stoi(v_hash.substr(6, 2), nullptr, 16));
		m_Data8[4] = static_cast<unsigned char>(std::stoi(v_hash.substr(8, 2), nullptr, 16));
		m_Data8[5] = static_cast<unsigned char>(std::stoi(v_hash.substr(10, 2), nullptr, 16));
		m_Data8[6] = static_cast<unsigned char>(std::stoi(v_hash.substr(12, 2), nullptr, 16));
		m_Data8[7] = static_cast<unsigned char>(std::stoi(v_hash.substr(14, 2), nullptr, 16));
		m_Data8[8] = static_cast<unsigned char>(std::stoi(v_hash.substr(16, 2), nullptr, 16));
		m_Data8[9] = static_cast<unsigned char>(std::stoi(v_hash.substr(18, 2), nullptr, 16));
		m_Data8[10] = static_cast<unsigned char>(std::stoi(v_hash.substr(20, 2), nullptr, 16));
		m_Data8[11] = static_cast<unsigned char>(std::stoi(v_hash.substr(22, 2), nullptr, 16));
		m_Data8[12] = static_cast<unsigned char>(std::stoi(v_hash.substr(24, 2), nullptr, 16));
		m_Data8[13] = static_cast<unsigned char>(std::stoi(v_hash.substr(26, 2), nullptr, 16));
		m_Data8[14] = static_cast<unsigned char>(std::stoi(v_hash.substr(28, 2), nullptr, 16));
		m_Data8[15] = static_cast<unsigned char>(std::stoi(v_hash.substr(30, 2), nullptr, 16));
	}

	inline std::size_t Hash() const
	{
		return (std::hash<unsigned long long>{}(m_Data64[0]) >> 1) ^ (std::hash<unsigned long long>{}(m_Data64[1]) << 2);
	}

	inline std::string ToString() const
	{
		std::stringstream sstream;

		const auto v_setw = std::setw(2);

		sstream << std::hex << std::setfill('0')
			<< v_setw << static_cast<int>(m_Data8[0])
			<< v_setw << static_cast<int>(m_Data8[1])
			<< v_setw << static_cast<int>(m_Data8[2])
			<< v_setw << static_cast<int>(m_Data8[3])
			<< '-'
			<< v_setw << static_cast<int>(m_Data8[4])
			<< v_setw << static_cast<int>(m_Data8[5])
			<< '-'
			<< v_setw << static_cast<int>(m_Data8[6])
			<< v_setw << static_cast<int>(m_Data8[7])
			<< '-'
			<< v_setw << static_cast<int>(m_Data8[8])
			<< v_setw << static_cast<int>(m_Data8[9])
			<< '-'
			<< v_setw << static_cast<int>(m_Data8[10])
			<< v_setw << static_cast<int>(m_Data8[11])
			<< v_setw << static_cast<int>(m_Data8[12])
			<< v_setw << static_cast<int>(m_Data8[13])
			<< v_setw << static_cast<int>(m_Data8[14])
			<< v_setw << static_cast<int>(m_Data8[15]);

		return sstream.str();
	}

	inline std::wstring ToWstring() const
	{
		std::wstringstream wstream;

		const auto v_setw = std::setw(2);

		wstream << std::hex << std::setfill(L'0')
			<< v_setw << static_cast<int>(m_Data8[0])
			<< v_setw << static_cast<int>(m_Data8[1])
			<< v_setw << static_cast<int>(m_Data8[2])
			<< v_setw << static_cast<int>(m_Data8[3])
			<< L'-'
			<< v_setw << static_cast<int>(m_Data8[4])
			<< v_setw << static_cast<int>(m_Data8[5])
			<< L'-'
			<< v_setw << static_cast<int>(m_Data8[6])
			<< v_setw << static_cast<int>(m_Data8[7])
			<< L'-'
			<< v_setw << static_cast<int>(m_Data8[8])
			<< v_setw << static_cast<int>(m_Data8[9])
			<< L'-'
			<< v_setw << static_cast<int>(m_Data8[10])
			<< v_setw << static_cast<int>(m_Data8[11])
			<< v_setw << static_cast<int>(m_Data8[12])
			<< v_setw << static_cast<int>(m_Data8[13])
			<< v_setw << static_cast<int>(m_Data8[14])
			<< v_setw << static_cast<int>(m_Data8[15]);

		return wstream.str();
	}

	inline void FromCString(const char* v_str)
	{
		if (strlen(v_str) != 36)
		{
			std::memset(m_Data8, 0, 16);
			return;
		}

		char v_str_cpy[37];
		v_str_cpy[36] = 0;
		std::memcpy(v_str_cpy, v_str, 36);

		m_Data8[15] = String::HexStrtolSafe(v_str_cpy + 34);
		m_Data8[14] = String::HexStrtolSafe(v_str_cpy + 32);
		m_Data8[13] = String::HexStrtolSafe(v_str_cpy + 30);
		m_Data8[12] = String::HexStrtolSafe(v_str_cpy + 28);
		m_Data8[11] = String::HexStrtolSafe(v_str_cpy + 26);
		m_Data8[10] = String::HexStrtolSafe(v_str_cpy + 24);

		m_Data8[9] = String::HexStrtolSafe(v_str_cpy + 21);
		m_Data8[8] = String::HexStrtolSafe(v_str_cpy + 19);

		m_Data8[7] = String::HexStrtolSafe(v_str_cpy + 16);
		m_Data8[6] = String::HexStrtolSafe(v_str_cpy + 14);

		m_Data8[5] = String::HexStrtolSafe(v_str_cpy + 11);
		m_Data8[4] = String::HexStrtolSafe(v_str_cpy + 9);

		m_Data8[3] = String::HexStrtolSafe(v_str_cpy + 6);
		m_Data8[2] = String::HexStrtolSafe(v_str_cpy + 4);
		m_Data8[1] = String::HexStrtolSafe(v_str_cpy + 2);
		m_Data8[0] = String::HexStrtolSafe(v_str_cpy);
	}

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
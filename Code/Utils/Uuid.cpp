#include "Uuid.hpp"

const SMUuid SMUuid::Null = SMUuid();

SMUuid::SMUuid()
{
	std::memset(m_Data8, 0, 16);
}

SMUuid::SMUuid(const std::string& uuid)
{
	this->FromString<std::string>(uuid);
}

SMUuid::SMUuid(const std::wstring& uuid)
{
	this->FromString<std::wstring>(uuid);
}

SMUuid::SMUuid(const char* uuid)
{
	this->FromCString(uuid);
}

SMUuid::SMUuid(const std::vector<long long>& longs)
{
	std::memcpy(&m_Data64, longs.data(), 16);
}

SMUuid::SMUuid(long long first, long long second, bool isBigEndian)
{
	m_Data64[0] = first;
	m_Data64[1] = second;

	if (isBigEndian)
	{
		std::reverse(m_Data8, m_Data8 + 16);
	}
}

bool SMUuid::IsNil() const noexcept
{
	return (m_Data64[0] == 0 && m_Data64[1] == 0);
}

SMUuid SMUuid::GenerateNamedStatic(const SMUuid& uuid, const std::string& some_string)
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

void SMUuid::GenerateNamed(const SMUuid& uuid, const std::string& str)
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

std::size_t SMUuid::Hash() const
{
	const std::uint64_t v_hash1 = std::hash<std::uint64_t>{}(m_Data64[0]);
	const std::uint64_t v_hash2 = std::hash<std::uint64_t>{}(m_Data64[1]);

	return (v_hash1 >> 1) ^ (v_hash2 << 2);
}

char* SMUuid::ToCString(char* v_beginning) const
{
	v_beginning = String::FromInteger<unsigned char, 16>(m_Data8[0], v_beginning);
	v_beginning = String::FromInteger<unsigned char, 16>(m_Data8[1], v_beginning);
	v_beginning = String::FromInteger<unsigned char, 16>(m_Data8[2], v_beginning);
	v_beginning = String::FromInteger<unsigned char, 16>(m_Data8[3], v_beginning);
	*v_beginning++ = '-';
	v_beginning = String::FromInteger<unsigned char, 16>(m_Data8[4], v_beginning);
	v_beginning = String::FromInteger<unsigned char, 16>(m_Data8[5], v_beginning);
	*v_beginning++ = '-';
	v_beginning = String::FromInteger<unsigned char, 16>(m_Data8[6], v_beginning);
	v_beginning = String::FromInteger<unsigned char, 16>(m_Data8[7], v_beginning);
	*v_beginning++ = '-';
	v_beginning = String::FromInteger<unsigned char, 16>(m_Data8[8], v_beginning);
	v_beginning = String::FromInteger<unsigned char, 16>(m_Data8[9], v_beginning);
	*v_beginning++ = '-';
	v_beginning = String::FromInteger<unsigned char, 16>(m_Data8[10], v_beginning);
	v_beginning = String::FromInteger<unsigned char, 16>(m_Data8[11], v_beginning);
	v_beginning = String::FromInteger<unsigned char, 16>(m_Data8[12], v_beginning);
	v_beginning = String::FromInteger<unsigned char, 16>(m_Data8[13], v_beginning);
	v_beginning = String::FromInteger<unsigned char, 16>(m_Data8[14], v_beginning);
	return String::FromInteger<unsigned char, 16>(m_Data8[15], v_beginning);
}

wchar_t* SMUuid::ToCStringW(wchar_t* v_beginning) const
{
	v_beginning = String::FromIntegerW<unsigned char, 16>(m_Data8[0], v_beginning);
	v_beginning = String::FromIntegerW<unsigned char, 16>(m_Data8[1], v_beginning);
	v_beginning = String::FromIntegerW<unsigned char, 16>(m_Data8[2], v_beginning);
	v_beginning = String::FromIntegerW<unsigned char, 16>(m_Data8[3], v_beginning);
	*v_beginning++ = '-';
	v_beginning = String::FromIntegerW<unsigned char, 16>(m_Data8[4], v_beginning);
	v_beginning = String::FromIntegerW<unsigned char, 16>(m_Data8[5], v_beginning);
	*v_beginning++ = '-';
	v_beginning = String::FromIntegerW<unsigned char, 16>(m_Data8[6], v_beginning);
	v_beginning = String::FromIntegerW<unsigned char, 16>(m_Data8[7], v_beginning);
	*v_beginning++ = '-';
	v_beginning = String::FromIntegerW<unsigned char, 16>(m_Data8[8], v_beginning);
	v_beginning = String::FromIntegerW<unsigned char, 16>(m_Data8[9], v_beginning);
	*v_beginning++ = '-';
	v_beginning = String::FromIntegerW<unsigned char, 16>(m_Data8[10], v_beginning);
	v_beginning = String::FromIntegerW<unsigned char, 16>(m_Data8[11], v_beginning);
	v_beginning = String::FromIntegerW<unsigned char, 16>(m_Data8[12], v_beginning);
	v_beginning = String::FromIntegerW<unsigned char, 16>(m_Data8[13], v_beginning);
	v_beginning = String::FromIntegerW<unsigned char, 16>(m_Data8[14], v_beginning);
	return String::FromIntegerW<unsigned char, 16>(m_Data8[15], v_beginning);
}

std::string SMUuid::ToString() const
{
	char v_buffer[37];
	v_buffer[36] = '\0';
	this->ToCString(v_buffer);

	return std::string(v_buffer, 36);
}

std::wstring SMUuid::ToWstring() const
{
	wchar_t v_buffer[37];
	v_buffer[36] = L'\0';
	this->ToCStringW(v_buffer);

	return std::wstring(v_buffer, 36);
}

void SMUuid::FromCString(const char* v_str)
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
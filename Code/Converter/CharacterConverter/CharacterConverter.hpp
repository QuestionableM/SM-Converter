#pragma once

#include "ObjectDatabase\UserDataReaders\UserCharacterData.hpp"
#include "Converter\ConvertError.hpp"

#pragma unmanaged

class CharacterConverter
{
public:
	static void ConvertToModel(const UserCharacterData& v_data, const std::wstring& filename, ConvertError& v_error);

private:
	static void ReadFromPath(const std::wstring& v_path);

	CharacterConverter() = default;
	CharacterConverter(const CharacterConverter&) = delete;
	CharacterConverter(CharacterConverter&&) = delete;
	~CharacterConverter() = default;
};

#pragma managed
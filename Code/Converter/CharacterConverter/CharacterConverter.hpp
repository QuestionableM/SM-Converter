#pragma once

#include "ObjectDatabase/UserDataReaders/UserCharacterData.hpp"
#include "Converter/ConvertError.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

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

SM_MANAGED_CODE
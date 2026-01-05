#pragma once

#include "Utils/clr_include.hpp"
#include "Utils/Hashing.hpp"

#include "UStd/UnmanagedUnorderedMap.hpp"
#include "UStd/UnmanagedString.hpp"

SM_UNMANAGED_CODE

class MaterialManager
{
	using MaterialMap = std::unordered_map<std::string, std::size_t, Hashing::StringHasher, std::equal_to<>>;
public:
	static void Initialize();

	//Returns the end of the material string
	static char* GetMaterialACStr(const std::string_view& material, char* pCString);
	static std::size_t GetMaterialIdx(const std::string_view& material);

	static void AppendMaterialIdx(std::string& outStr, const std::string_view& material);

private:
	static MaterialMap m_materialStorage;
};

SM_MANAGED_CODE
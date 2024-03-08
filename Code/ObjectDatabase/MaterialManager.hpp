#pragma once

#include "Utils/clr_include.hpp"

#include "UStd/UnmanagedUnorderedMap.hpp"
#include "UStd/UnmanagedString.hpp"

SM_UNMANAGED_CODE

class MaterialManager
{
	using MaterialMap = std::unordered_map<std::string, std::size_t>;
public:
	static void Initialize();

	//Returns the end of the material string
	static char* GetMaterialACStr(const std::string& mat_name, char* v_ptr);

	static std::string GetMaterialA(const std::string& mat_name);
	static std::wstring GetMaterialW(const std::string& mat_name);

private:
	static MaterialMap m_materialStorage;
};

SM_MANAGED_CODE
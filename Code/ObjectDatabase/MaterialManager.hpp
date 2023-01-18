#pragma once

#include "Utils\UnmanagedUnorderedMap.hpp"
#include "Utils\UnmanagedString.hpp"

#pragma unmanaged

class MaterialManager
{
	using MaterialMap = std::unordered_map<std::string, std::string>;
public:
	static void Initialize();

	static std::string GetMaterialA(const std::string& mat_name);
	static std::wstring GetMaterialW(const std::string& mat_name);

private:
	static MaterialMap m_materialStorage;
};

#pragma managed
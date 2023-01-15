#pragma once

#include <unordered_map>
#include <string>

#pragma unmanaged

class MaterialManager
{
	using MaterialMap = std::unordered_map<std::wstring, std::wstring>;
public:
	static void Initialize();

	static std::wstring GetMaterialW(const std::wstring& mat_name);
	static std::string GetMaterialA(const std::wstring& mat_name);

private:
	static MaterialMap m_materialStorage;
};

#pragma managed
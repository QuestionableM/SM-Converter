#include "MaterialManager.hpp"

#include "ObjectDatabase\DatabaseConfig.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\Json.hpp"

#pragma unmanaged

MaterialManager::MaterialMap MaterialManager::m_materialStorage = {};

void MaterialManager::Initialize()
{
	simdjson::dom::document v_doc;
	if (!JsonReader::LoadParseSimdjsonCommentsC(DatabaseConfig::MaterialMapPath.data(), v_doc, simdjson::dom::element_type::OBJECT))
		return;

	for (const auto v_object : v_doc.root().get_object())
	{
		if (!v_object.value.is_string()) continue;

		const std::wstring v_key = String::ToWide(v_object.key);
		const std::wstring v_value = String::ToWide(v_object.value.get_string());

		if (m_materialStorage.find(v_key) != m_materialStorage.end())
			continue;

		m_materialStorage.insert(std::make_pair(v_key, v_value));
	}
}

std::wstring MaterialManager::GetMaterialW(const std::wstring& mat_name)
{
	const MaterialMap::const_iterator iter = m_materialStorage.find(mat_name);
	if (iter != m_materialStorage.end())
		return L"m" + iter->second;

	DebugOutL("Couldn't find the specified material: ", mat_name);

	return L"m1";
}

std::string MaterialManager::GetMaterialA(const std::wstring& mat_name)
{
	return String::ToUtf8(MaterialManager::GetMaterialW(mat_name));
}
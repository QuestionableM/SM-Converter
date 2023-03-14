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
		if (!v_object.value.is_number()) continue;

		std::string v_key(v_object.key.data(), v_object.key.size());
		const std::size_t v_value = JsonReader::GetNumber<std::size_t>(v_object.value);

		if (m_materialStorage.find(v_key) != m_materialStorage.end())
			continue;

		m_materialStorage.emplace(std::move(v_key), v_value);
	}
}

char* MaterialManager::GetMaterialACStr(const std::string& mat_name, char* v_ptr)
{
	*v_ptr++ = 'm';

	const MaterialMap::const_iterator v_iter = m_materialStorage.find(mat_name);
	if (v_iter != m_materialStorage.end())
		v_ptr = String::FromInteger<std::size_t>(v_iter->second, v_ptr);
	else
		*v_ptr++ = '1';

	return v_ptr;
}

std::string MaterialManager::GetMaterialA(const std::string& mat_name)
{
	const MaterialMap::const_iterator v_iter = m_materialStorage.find(mat_name);
	if (v_iter != m_materialStorage.end())
		return "m" + std::to_string(v_iter->second);

	DebugOutL("Couldn't find the specified material: ", mat_name);

	return "m1";
}

std::wstring MaterialManager::GetMaterialW(const std::string& mat_name)
{
	return String::ToWide(MaterialManager::GetMaterialA(mat_name));
}
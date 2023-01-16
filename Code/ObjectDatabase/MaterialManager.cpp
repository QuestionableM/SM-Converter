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

		const std::string v_key(v_object.key.data(), v_object.key.size());

		const std::string_view v_value_view = v_object.value.get_string();
		const std::string v_value(v_value_view.data(), v_value_view.size());

		if (m_materialStorage.find(v_key) != m_materialStorage.end())
			continue;

		m_materialStorage.insert(std::make_pair(v_key, v_value));
	}
}

std::string MaterialManager::GetMaterialA(const std::string& mat_name)
{
	const MaterialMap::const_iterator v_iter = m_materialStorage.find(mat_name);
	if (v_iter != m_materialStorage.end())
		return "m" + v_iter->second;

	DebugOutL("Couldn't find the specified material: ", mat_name);

	return "m1";
}

std::wstring MaterialManager::GetMaterialW(const std::string& mat_name)
{
	return String::ToWide(MaterialManager::GetMaterialA(mat_name));
}
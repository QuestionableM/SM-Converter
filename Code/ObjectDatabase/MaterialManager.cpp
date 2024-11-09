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
	if (!JsonReader::LoadParseSimdjsonCommentsC(
		DatabaseConfig::MaterialMapPath.data(),
		v_doc,
		simdjson::dom::element_type::OBJECT))
	{
		return;
	}

	for (const auto& v_object : v_doc.root().get_object().value_unsafe())
	{
		if (!v_object.value.is_number() || m_materialStorage.contains(v_object.key)) continue;

		m_materialStorage.emplace(
			v_object.key,
			JsonReader::GetNumber<std::size_t>(v_object.value)
		);
	}
}

char* MaterialManager::GetMaterialACStr(const std::string& mat_name, char* v_ptr)
{
	*v_ptr++ = 'm';

	const auto v_iter = m_materialStorage.find(mat_name);
	if (v_iter != m_materialStorage.end())
		v_ptr = String::FromInteger<std::size_t>(v_iter->second, v_ptr);
	else
		*v_ptr++ = '1';

	return v_ptr;
}

std::size_t MaterialManager::GetMaterialIdx(const std::string& mat_name)
{
	const auto v_iter = m_materialStorage.find(mat_name);
	if (v_iter != m_materialStorage.end())
		return v_iter->second;
	else
		return 1;
}

void MaterialManager::AppendMaterialIdx(std::string& outStr, const std::string& mat_name)
{
	outStr.append(" m", 2);
	String::AppendIntegerToString(outStr, MaterialManager::GetMaterialIdx(mat_name));
}
#include "DecalsetListReader.hpp"

#include "ObjectDatabase\Readers\DecalsetReader.hpp"
#include "ObjectDatabase\KeywordReplacer.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"

#pragma unmanaged

void DecalsetListReader::Load(const simdjson::dom::element& j_data, SMMod* mod, bool add_to_global_db)
{
	if (!j_data.is_array()) return;

	std::wstring v_pathStr;

	for (const auto v_decal_set : j_data.get_array().value_unsafe())
	{
		if (!v_decal_set.is_object()) continue;

		const auto v_set_path = v_decal_set["set"];
		if (!v_set_path.is_string()) continue;

		String::ToWideRef(v_set_path.get_string().value_unsafe(), v_pathStr);
		KeywordReplacer::ReplaceKeyR(v_pathStr);

		DecalsetReader::LoadFromFile(v_pathStr, mod, add_to_global_db);
	}
}
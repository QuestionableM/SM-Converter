#include "CustomGameMod.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"
#include "Utils\Json.hpp"

#pragma unmanaged

bool CustomGame::CheckSurvivalContent()
{
	const std::wstring v_config_path = m_Directory + L"/config.json";

	simdjson::dom::document v_doc;
	if (!JsonReader::LoadParseSimdjsonCommentsC(v_config_path, v_doc, simdjson::dom::element_type::OBJECT))
		return false;

	const auto v_base_game_content = v_doc.root()["baseGameContent"];
	if (!v_base_game_content.is_string()) return false;

	return (strcmp(v_base_game_content.get_c_str(), "Survival") == 0);
}

void CustomGame::LoadObjectDatabase()
{
	KeywordReplacer::SetModData(m_Directory, m_Uuid);

	m_shouldUseGameContent = this->CheckSurvivalContent();

	const std::wstring v_shapedb_path = m_Directory + L"/Objects/Database/shapesets.shapedb";
	SMMod::LoadShapeSetList(v_shapedb_path, this, false);

	const std::wstring v_assetdb_path = m_Directory + L"/Terrain/Database/assetsets.assetdb";
	SMMod::LoadAssetSetList(v_assetdb_path, this, false);

	const std::wstring v_hvsdb_path = m_Directory + L"/Harvestables/Database/harvestablesets.harvestabledb";
	SMMod::LoadHarvestableSetList(v_hvsdb_path, this, false);
}
#include "BlocksAndPartsMod.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

static const std::wstring g_ShapeSetDbExtensions[2] = { L"json", L"shapedb" };
bool BlocksAndPartsMod::GetShapeSetDatabaseFile(const std::wstring& mod_folder, std::wstring& r_shapedb_path)
{
	const std::wstring lNearFullPath = mod_folder + L"/Objects/Database/shapesets.";

	for (__int8 a = 0; a < 2; a++)
	{
		const std::wstring lFullShapedbPath = lNearFullPath + g_ShapeSetDbExtensions[a];

		if (File::Exists(lFullShapedbPath))
		{
			r_shapedb_path = lFullShapedbPath;
			return true;
		}
	}

	return false;
}

void BlocksAndPartsMod::LoadShapeSetDatabase(const std::wstring& path, SMMod* pMod)
{
	simdjson::dom::document v_shapedb_doc;
	if (!JsonReader::LoadParseSimdjsonCommentsC(path, v_shapedb_doc, simdjson::dom::element_type::OBJECT))
		return;

	const auto v_shapeset_list = v_shapedb_doc.root()["shapeSetList"];
	if (!v_shapeset_list.is_array()) return;

	for (const auto v_shapeset : v_shapeset_list.get_array())
	{
		if (!v_shapeset.is_string()) continue;

		std::wstring v_shapeset_path = String::ToWide(v_shapeset.get_string());
		KeywordReplacer::ReplaceKeyR(v_shapeset_path);

		pMod->LoadFile(v_shapeset_path);
	}
}

void BlocksAndPartsMod::LoadObjectDatabase()
{
	KeywordReplacer::SetModData(m_Directory, m_Uuid);

	std::wstring lShapeSetDbPath;
	if (BlocksAndPartsMod::GetShapeSetDatabaseFile(m_Directory, lShapeSetDbPath))
	{
		BlocksAndPartsMod::LoadShapeSetDatabase(lShapeSetDbPath, this);
	}
	else
	{
		const std::wstring l_DatabaseFolder = m_Directory + L"/Objects/Database/ShapeSets";
		if (File::Exists(l_DatabaseFolder))
		{
			this->ScanDatabaseFolder(l_DatabaseFolder);
		}
	}
}
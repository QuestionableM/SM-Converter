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

void BlocksAndPartsMod::LoadObjectDatabase()
{
	KeywordReplacer::SetModData(m_Directory, m_Uuid);

	std::wstring v_shapedb_path;
	if (BlocksAndPartsMod::GetShapeSetDatabaseFile(m_Directory, v_shapedb_path))
	{
		SMMod::LoadShapeSetList(v_shapedb_path, this, true);
	}
	else
	{
		const std::wstring v_shapesets_folder = m_Directory + L"/Objects/Database/ShapeSets";
		if (File::Exists(v_shapesets_folder))
		{
			this->ScanDatabaseFolder(v_shapesets_folder, true);
		}
	}
}
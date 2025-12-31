#include "BlocksAndPartsMod.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\File.hpp"

SM_UNMANAGED_CODE

BlocksAndPartsMod::BlocksAndPartsMod(
	const std::wstring& name,
	const std::wstring& directory,
	const SMUuid& uuid,
	std::uint64_t workshop_id,
	bool isLocal
)
	: SMMod(
		name,
		directory,
		uuid,
		workshop_id,
		isLocal
	)
{
	SMMod::ModStorage.emplace(m_Uuid, this);
	SMMod::ModVector.push_back(this);
}

static const std::wstring_view g_shapeSetDbExtensions[2] = { L"json", L"shapedb" };
bool BlocksAndPartsMod::GetShapeSetDatabaseFile(const std::wstring& mod_folder, std::wstring& r_shapedb_path)
{
	const std::wstring v_nearFullPath = mod_folder + L"/Objects/Database/shapesets.";
	std::wstring v_fullShapeDbPath;

	for (const std::wstring_view& v_curShapeDbExtension : g_shapeSetDbExtensions)
	{
		v_fullShapeDbPath = v_nearFullPath;
		v_fullShapeDbPath.append(v_curShapeDbExtension);
		if (!File::Exists(v_fullShapeDbPath)) continue;

		r_shapedb_path = std::move(v_fullShapeDbPath);
		return true;
	}

	return false;
}

void BlocksAndPartsMod::LoadObjectDatabase()
{
	KeywordReplacer::SetModData(m_Directory, m_Uuid);

	std::wstring v_shapedbPath;
	if (BlocksAndPartsMod::GetShapeSetDatabaseFile(m_Directory, v_shapedbPath))
	{
		SMMod::LoadShapeSetList(v_shapedbPath, this, true);
	}
	else
	{
		const std::wstring v_shapesetsFolder = m_Directory + L"/Objects/Database/ShapeSets";
		if (!File::Exists(v_shapesetsFolder)) return;

		this->ScanDatabaseFolder(v_shapesetsFolder, true);
	}
}
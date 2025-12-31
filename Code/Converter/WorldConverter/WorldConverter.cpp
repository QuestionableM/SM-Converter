#include "WorldConverter.hpp"

#include "ObjectDatabase\UserDataReaders\TileFolderReader.hpp"
#include "ObjectDatabase\Mods\CustomGameSwitch.hpp"
#include "ObjectDatabase\DatabaseConfig.hpp"

#include "Converter\WorldConverter\World.hpp"
#include "Utils\File.hpp"

SM_UNMANAGED_CODE

void WorldConverter::WriteToFile(
	class SMWorld* pWorld,
	const std::wstring& world_name,
	ConvertError& error)
{
	if (error) return; //Error check

	const std::wstring v_world_out_dir(DatabaseConfig::WorldOutputFolder);
	if (!File::CreateDirectorySafe(v_world_out_dir))
	{
		error.setError(1, "Couldn't create the main output directory");
		return;
	}

	const std::wstring v_world_dir_path = v_world_out_dir + L"/" + world_name;
	if (!File::CreateDirectorySafe(v_world_dir_path))
	{
		error.setError(1, "Couldn't create the tile output directory");
		return;
	}

	pWorld->WriteToFile(v_world_dir_path + L"/", world_name);
}

void WorldConverter::ConvertToModel(
	const std::wstring& path,
	const std::wstring& name,
	ConvertError& error,
	CustomGame* pCustomGame)
{
	if (!File::IsRegularFile(path))
	{
		error.setError(1, "The specified path leads to a directory");
		return;
	}

	TileFolderReader::InitializeTileKeys();

	SMWorld* v_outputWorld = nullptr;
	if (pCustomGame)
	{
		//The original content will be set back automatically by the SMModCustomGameSwitch destructor
		SMModCustomGameSwitch<false, true> v_contentSwitch;
		v_contentSwitch.MergeContent(pCustomGame);
		
		v_outputWorld = SMWorld::LoadFromFile<false>(path, error);
	}
	else
	{
		v_outputWorld = SMWorld::LoadFromFile<false>(path, error);
	}

	if (v_outputWorld)
	{
		WorldConverter::WriteToFile(v_outputWorld, name, error);
		delete v_outputWorld;
	}

	ModelStorage::ClearStorage();
}
#include "WorldConverter.hpp"

#include "ObjectDatabase\Mods\CustomGameSwitch.hpp"
#include "ObjectDatabase\DatabaseConfig.hpp"

#include "Converter\WorldConverter\World.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

void WorldConverter::WriteToFile(class SMWorld* v_world, const std::wstring& world_name, ConvertError& v_error)
{
	if (v_error) return; //Error check

	const std::wstring v_world_out_dir = std::wstring(DatabaseConfig::WorldOutputFolder.data());
	if (!File::CreateDirectorySafe(v_world_out_dir))
	{
		v_error = ConvertError(1, L"Couldn't create the main output directory");
		return;
	}

	const std::wstring v_world_dir_path = v_world_out_dir + L"/" + world_name;
	if (!File::CreateDirectorySafe(v_world_dir_path))
	{
		v_error = ConvertError(1, L"Couldn't create the tile output directory");
		return;
	}

	v_world->WriteToFile(v_world_dir_path + L"/", world_name);
}

void WorldConverter::ConvertToModel(const std::wstring& path, const std::wstring& name, ConvertError& v_error, CustomGame* v_custom_game)
{
	if (!File::IsRegularFile(path))
	{
		v_error = ConvertError(1, L"The specified path leads to a directory");
		return;
	}

	SMWorld* v_output_world = nullptr;
	if (v_custom_game)
	{
		//The original content will be set back automatically by the SMModCustomGameSwitch destructor
		SMModCustomGameSwitch<false, true> v_content_switch;
		v_content_switch.MergeContent(v_custom_game);
		
		v_output_world = SMWorld::LoadFromFile<false>(path, v_error);
	}
	else
	{
		v_output_world = SMWorld::LoadFromFile<false>(path, v_error);
	}

	if (v_output_world)
	{
		WorldConverter::WriteToFile(v_output_world, name, v_error);
		delete v_output_world;
	}

	ModelStorage::ClearStorage();
}
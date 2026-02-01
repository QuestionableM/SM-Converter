#include "ObjectDatabase/UserDataReaders/UserObjectFolderReader.hpp"
#include "ObjectDatabase/UserDataReaders/BlueprintFolderReader.hpp"
#include "ObjectDatabase/UserDataReaders/WorldFolderReader.hpp"
#include "ObjectDatabase/UserDataReaders/TileFolderReader.hpp"

#include "Converter/BlueprintConverter/BlueprintConverter.hpp"
#include "Converter/WorldConverter/WorldConverter.hpp"
#include "Converter/TileConverter/TileConverter.hpp"

#include "ObjectDatabase/DatabaseConfig.hpp"
#include "ObjectDatabase/ObjectDatabase.hpp"

#include "Utils/WinInclude.hpp"
#include "Utils/Console.hpp"

#include "cli_parser.hpp"

#include <iostream>
#include <string>

template<typename T>
static void handleUserItemListPrinter()
{
	AttachDebugConsole();

	if (T::Storage.empty())
	{
		DebugErrorL("No items found");
		return;
	}

	for (const auto* v_pCurItem : T::Storage)
		DebugOutL("Path: ", v_pCurItem->path, ", Name: ", v_pCurItem->name);
}

void handleCustomGameList()
{
	DatabaseLoader::LoadDatabase();
	AttachDebugConsole();

	if (SMMod::GetCustomGames().empty())
	{
		DebugErrorL("No custom games found");
		return;
	}

	for (const auto* v_pCurItem : SMMod::GetCustomGames())
		DebugOutL("UUID: ", v_pCurItem->GetUuid().toString(), ", Name: ", v_pCurItem->GetName(), ", Path: ", v_pCurItem->GetDirectory());
}

int main(const int argc, const char* argv[])
{
	if (argc < 2)
	{
		std::cout << "SMConverterCLI -type [Blueprint|Tile|World]\n";
		std::cout << "Type -help for more info\n";
		return -1;
	}

	// Skip the first argument (the path)
	ArgumentParser v_parser(argc - 1, argv + 1);
	if (!v_parser.processArgs())
		return -1;

	if (v_parser.actionFlagSet(ConverterActionFlag::DebugFlag))
		AttachDebugConsole();

	if (v_parser.anyActionFlagSet(ConverterActionFlag::List))
	{
		DatabaseConfig::ReadConfig();
		UserObjectFolderReader::ReadFromConfig();

		if (v_parser.actionFlagSet(ConverterActionFlag::ListBlueprints))
			handleUserItemListPrinter<BlueprintFolderReader>();
		else if (v_parser.actionFlagSet(ConverterActionFlag::ListTiles))
			handleUserItemListPrinter<TileFolderReader>();
		else if (v_parser.actionFlagSet(ConverterActionFlag::ListWorlds))
			handleUserItemListPrinter<WorldFolderReader>();
		else if (v_parser.actionFlagSet(ConverterActionFlag::ListCustomGames))
			handleCustomGameList();

		return 0;
	}

	if (v_parser.actionFlagSet(ConverterActionFlag::TypeSpecified))
	{
		if (!v_parser.actionFlagSet(ConverterActionFlag::PathSpecified))
		{
			AttachDebugConsole();
			DebugOutL("You must specify the path to an object you want to convert\n");
			return -1;
		}

		if (!v_parser.actionFlagSet(ConverterActionFlag::NameSpecified))
		{
			AttachDebugConsole();
			DebugOutL("You must specify the output name of an object\n");
			return -1;
		}

		DatabaseLoader::InitializeDatabase();
		DatabaseLoader::LoadDatabase();

		CustomGame* v_pCustomGame = nullptr;
		ConvertError v_convError;


		if (v_parser.actionFlagSet(ConverterActionFlag::CustomgameUuidSpecified))
		{
			v_pCustomGame = SMMod::GetCustomGameFromUuid(v_parser.m_customGameUuid);
			if (!v_pCustomGame)
			{
				AttachDebugConsole();
				DebugOutL("Custom game with uuid: ", v_parser.m_customGameUuid.toString(), " could not be found");

				return -1;
			}
		}

		// Apply the converter settings to the converter context
		v_parser.applySettings();

		switch (v_parser.m_convType)
		{
		case ConverterType::Blueprint:
			BlueprintConv::ConvertToModel(v_parser.m_itemPath, v_parser.m_itemName, v_convError, v_pCustomGame);
			break;
		case ConverterType::Tile:
			TileConv::ConvertToModel(v_parser.m_itemPath, v_parser.m_itemName, v_convError, v_pCustomGame);
			break;
		case ConverterType::World:
			WorldConverter::ConvertToModel(v_parser.m_itemPath, v_parser.m_itemName, v_convError, v_pCustomGame);
			break;
		default:
			return -1;
		}

		if (v_convError)
		{
			AttachDebugConsole();
			DebugErrorL("Convert Error -> ", v_convError.getErrorMsg());
			return -1;
		}
	}
	else
	{
		AttachDebugConsole();
		DebugOutL("Specify the object type with \"-type\" argument");
		return -1;
	}

	return 0;
}
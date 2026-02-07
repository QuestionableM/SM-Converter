#include "ObjectDatabase/UserDataReaders/UserObjectFolderReader.hpp"
#include "ObjectDatabase/UserDataReaders/BlueprintFolderReader.hpp"
#include "ObjectDatabase/UserDataReaders/WorldFolderReader.hpp"
#include "ObjectDatabase/UserDataReaders/TileFolderReader.hpp"

#include "Converter/BlueprintConverter/BlueprintConverter.hpp"
#include "Converter/WorldConverter/WorldConverter.hpp"
#include "Converter/TileConverter/TileConverter.hpp"

#include "ObjectDatabase/DatabaseConfig.hpp"
#include "ObjectDatabase/ObjectDatabase.hpp"

#include "Utils/Console.hpp"
#include "cli_parser.hpp"

#include "BuildInfo.hpp"

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

static void handleCustomGameList()
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

static void printMainArguments(const bool printHeader)
{
	DebugOutL(0b1101_fg, "\t-type          ", 0b1110_fg, " [Blueprint | Tile | World] - specifies the converter type");
	DebugOutL(0b1101_fg, "\t-path          ", 0b1110_fg, " <path to file> - path to the file that will be converted");
	DebugOutL(0b1101_fg, "\t-name          ", 0b1110_fg, " <output name> - the name of the output file");
	DebugOutL(0b1101_fg, "\t-customgameuuid", 0b1110_fg, " <uuid> - custom game content uuid to use (use -customgamelist to find the custom game uuid you need)");
}

static void printGenericArguments(const bool printHeader)
{
	if (printHeader)
		DebugOutL("\nGeneric arguments (Applicable to Blueprint, Tile, and World converter):");

	DebugOutL(0b1101_fg, "\t-exportmaterials ", 0b1110_fg, " - enables textures for the output model.");
	DebugOutL(0b1101_fg, "\t-exportuvs       ", 0b1110_fg, " - enables texture UVs for the output model.");
	DebugOutL(0b1101_fg, "\t-exportnormals   ", 0b1110_fg, " - enables normals for the output model.");
	DebugOutL(0b1101_fg, "\t-deduplicateverts", 0b1110_fg, " - joins the vertices that are close to each other to save space. (breaks normals for blender 5.0+)");
}

static void printTileOrWorldArguments(const bool printHeader)
{
	if (printHeader)
		DebugOutL("\nArguments for Tile and World converter:");

	DebugOutL(0b1101_fg, "\t-exportclutter         ", 0b1110_fg, " - enables clutter (grass, small rocks, etc.)");
	DebugOutL(0b1101_fg, "\t-exportassets          ", 0b1110_fg, " - enables assets (old trees, rocks, bushes, etc.)");
	DebugOutL(0b1101_fg, "\t-exportprefabs         ", 0b1110_fg, " - enables prefabs");
	DebugOutL(0b1101_fg, "\t-exportblueprints      ", 0b1110_fg, " - enables blueprints");
	DebugOutL(0b1101_fg, "\t-exportkinematics      ", 0b1110_fg, " - enables kinematics");
	DebugOutL(0b1101_fg, "\t-exportharvestables    ", 0b1110_fg, " - enables harvestables (destructible trees, rocks, etc.)");
	DebugOutL(0b1101_fg, "\t-exportdecals          ", 0b1110_fg, " - enables decals (WIP)");
	DebugOutL(0b1101_fg, "\t-exportgroundtextures  ", 0b1110_fg, " - enables ground textures");
	DebugOutL(0b1101_fg, "\t-export8kgroundtextures", 0b1110_fg, " - enables high resolution ground textures");
}

static void printBlueprintArguments(const bool printHeader)
{
	if (printHeader)
		DebugOutL("\nArguments for Blueprint converter:");

	DebugOutL(0b1101_fg, "\t-septype", 0b1110_fg, " [none | all | shapes | joints | uuid | color | uuidandcolor] (default is none) - object separation type");
	DebugOutL(0b1101_fg, "\t\tnone        ", 0b1110_fg, " - no object separation");
	DebugOutL(0b1101_fg, "\t\tall         ", 0b1110_fg, " - puts every single shape into a unique group");
	DebugOutL(0b1101_fg, "\t\tshapes      ", 0b1110_fg, " - puts every single shape (groups of objects) into a unqiue group");
	DebugOutL(0b1101_fg, "\t\tjoints      ", 0b1110_fg, " - puts every single shape connected with a joint into a unique group");
	DebugOutL(0b1101_fg, "\t\tuuid        ", 0b1110_fg, " - groups all the shapes by uuid");
	DebugOutL(0b1101_fg, "\t\tcolor       ", 0b1110_fg, " - groups all the shapes by color");
	DebugOutL(0b1101_fg, "\t\tuuidandcolor", 0b1110_fg, " - groups all the shapes by uuid and color");
}

static void printOtherArguments(const bool printHeader)
{
	if (printHeader)
		DebugOutL("\nOther arguments:");

	DebugOutL(0b1101_fg, "\t-debug         ", 0b1110_fg, " - enables additional console output");
	DebugOutL(0b1101_fg, "\t-blueprintlist ", 0b1110_fg, " - lists all the blueprints accessible in the game");
	DebugOutL(0b1101_fg, "\t-tilelist      ", 0b1110_fg, " - lists all the tiles accessible in the game");
	DebugOutL(0b1101_fg, "\t-worldlist     ", 0b1110_fg, " - lists all the worlds accessible in the game");
	DebugOutL(0b1101_fg, "\t-customgamelist", 0b1110_fg, " - lists all the custom games accessible to the converter");
}

int main(const int argc, const char* argv[])
{
	if (argc < 2)
	{
		AttachDebugConsole();

		DebugOutL("SMConverterCLI Version: ", SM_CONVERTER_BUILD_VERSION);

		printMainArguments(true);
		printGenericArguments(true);
		printTileOrWorldArguments(true);
		printBlueprintArguments(true);
		printOtherArguments(true);

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
			DebugOutL("You must specify the path to an object you want to convert with the -path <path to file> argument\n");
			return -1;
		}

		if (!v_parser.actionFlagSet(ConverterActionFlag::NameSpecified))
		{
			AttachDebugConsole();
			DebugOutL("You must specify the output name of an object with -name <output name>\n");
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
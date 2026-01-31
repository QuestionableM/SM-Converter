#include "cli_parser.hpp"

#include "Converter/BlueprintConverter/BlueprintConverter.hpp"
#include "Utils/String.hpp"

static ArgumentParserCallback g_callbacks[] =
{
	{ "-debug"         , &ArgumentParser::actionFlagSetter<ConverterActionFlag::DebugFlag>       },
	{ "-type"          , &ArgumentParser::argType                                                },
	{ "-path"          , &ArgumentParser::argPath                                                },
	{ "-name"          , &ArgumentParser::argName                                                },
	{ "-blueprintlist" , &ArgumentParser::actionFlagSetter<ConverterActionFlag::ListBlueprints>  },
	{ "-tilelist"      , &ArgumentParser::actionFlagSetter<ConverterActionFlag::ListTiles>       },
	{ "-worldlist"     , &ArgumentParser::actionFlagSetter<ConverterActionFlag::ListWorlds>      },
	{ "-customgamelist", &ArgumentParser::actionFlagSetter<ConverterActionFlag::ListCustomGames> },
	{ "-customgameuuid", &ArgumentParser::argCustomGameUuid                                      },

	// Generic settings

	{ "-exportmaterials" , &ArgumentParser::exportFlagSetter<ConverterType::Unknown, ConverterExportSettings::ExportMaterials>     },
	{ "-exportuvs"       , &ArgumentParser::exportFlagSetter<ConverterType::Unknown, ConverterExportSettings::ExportUvs>           },
	{ "-exportnormals"   , &ArgumentParser::exportFlagSetter<ConverterType::Unknown, ConverterExportSettings::ExportNormals>       },
	{ "-deduplicateverts", &ArgumentParser::exportFlagSetter<ConverterType::Unknown, ConverterExportSettings::DeduplicateVertices> },

	// Tile / World specific settings

	{ "-exportclutter"         , &ArgumentParser::exportFlagSetter<ConverterType::Tile | ConverterType::World, ConverterExportSettings::ExportClutter>          },
	{ "-exportassets"          , &ArgumentParser::exportFlagSetter<ConverterType::Tile | ConverterType::World, ConverterExportSettings::ExportAssets>           },
	{ "-exportprefabs"         , &ArgumentParser::exportFlagSetter<ConverterType::Tile | ConverterType::World, ConverterExportSettings::ExportPrefabs>          },
	{ "-exportblueprints"      , &ArgumentParser::exportFlagSetter<ConverterType::Tile | ConverterType::World, ConverterExportSettings::ExportBlueprints>       },
	{ "-exportkinematics"      , &ArgumentParser::exportFlagSetter<ConverterType::Tile | ConverterType::World, ConverterExportSettings::ExportKinematics>       },
	{ "-exportharvestables"    , &ArgumentParser::exportFlagSetter<ConverterType::Tile | ConverterType::World, ConverterExportSettings::ExportHarvestables>     },
	{ "-exportdecals"          , &ArgumentParser::exportFlagSetter<ConverterType::Tile | ConverterType::World, ConverterExportSettings::ExportDecals>           },
	{ "-exportgroundtextures"  , &ArgumentParser::exportFlagSetter<ConverterType::Tile | ConverterType::World, ConverterExportSettings::ExportGroundTextures>   },
	{ "-export8kgroundtextures", &ArgumentParser::exportFlagSetter<ConverterType::Tile | ConverterType::World, ConverterExportSettings::Export8KGroundTextures> },

	// Blueprint specific settings

	{ "-septype", &ArgumentParser::argSepType }
};

ArgumentParser::ArgumentParser(
	const int argc,
	const char* argv[]
)
	: m_exportSettings(ConverterExportSettings::None)
	, m_actionFlags(ConverterActionFlag::None)
	, m_convType(ConverterType::Unknown)
	, m_separationType(BPObjectSep_None)
	, m_itemPath()
	, m_itemName()
	, m_argCount(argc)
	, m_curArg(0)
	, m_ppArgs(argv)
	, m_customGameUuid()
{}

bool ArgumentParser::processArgs()
{
	for (m_curArg = 0; m_curArg < m_argCount; m_curArg++)
	{
		const auto v_pCallback = this->findCallback(m_ppArgs[m_curArg]);
		if (!v_pCallback)
		{
			AttachDebugConsole();
			DebugOutL("Unknown argument: ", m_ppArgs[m_curArg]);

			return false;
		}

		const auto v_result = (this->*v_pCallback->callback)();
		if (!v_result)
			return false;
	}

	return true;
}

std::size_t ArgumentParser::remainingArgs()
{
	return m_argCount - m_curArg - 1;
}

const ArgumentParserCallback* ArgumentParser::findCallback(const char* arg)
{
	for (const auto& v_curCallback : g_callbacks)
		if (std::strcmp(arg, v_curCallback.argName) == 0)
			return &v_curCallback;

	return nullptr;
}

bool ArgumentParser::actionFlagSet(const ConverterActionFlag flag) const noexcept
{
	return (m_actionFlags & flag) == flag;
}

bool ArgumentParser::anyActionFlagSet(const ConverterActionFlag flag) const noexcept
{
	return (m_actionFlags & flag) != ConverterActionFlag::None;
}

bool ArgumentParser::exportFlagSet(const ConverterExportSettings flag) const noexcept
{
	return (m_exportSettings & flag) == flag;
}

bool ArgumentParser::argType()
{
	if (remainingArgs() < 1)
	{
		AttachDebugConsole();
		DebugOutL("Usage: -type [Blueprint|Tile|World]");

		return false;
	}

	m_curArg++;

	if (std::strcmp(m_ppArgs[m_curArg], "Blueprint") == 0)
		m_convType = ConverterType::Blueprint;
	else if (std::strcmp(m_ppArgs[m_curArg], "Tile") == 0)
		m_convType = ConverterType::Tile;
	else if (std::strcmp(m_ppArgs[m_curArg], "World") == 0)
		m_convType = ConverterType::World;
	else
	{

		AttachDebugConsole();
		DebugOutL("Unknown conversion type: ", m_ppArgs[m_curArg]);

		return false;
	}

	m_actionFlags |= ConverterActionFlag::TypeSpecified;
	return true;
}

bool ArgumentParser::argPath()
{
	if (remainingArgs() < 1)
	{
		AttachDebugConsole();
		DebugOutL("Usage: -path <path_to_item>");

		return false;
	}

	m_curArg++;
	String::ToWideRef(m_ppArgs[m_curArg], m_itemPath);

	m_actionFlags |= ConverterActionFlag::PathSpecified;
	return true;
}

bool ArgumentParser::argName()
{
	if (remainingArgs() < 1)
	{
		AttachDebugConsole();
		DebugOutL("Usage: -name <name_of_item>");

		return false;
	}

	m_curArg++;
	String::ToWideRef(m_ppArgs[m_curArg], m_itemName);

	m_actionFlags |= ConverterActionFlag::NameSpecified;
	return true;
}

bool ArgumentParser::argSepType()
{
	if (remainingArgs() < 1)
	{
		AttachDebugConsole();
		DebugOutL("Usage: -septype [none|all|shapes|joints|uuid|color|uuidandcolor]");

		return false;
	}

	if (m_convType != ConverterType::Blueprint)
	{
		AttachDebugConsole();
		DebugOutL("Error: -septype option can only be used with Blueprint type");

		return false;
	}

	m_curArg++;

	if (std::strcmp(m_ppArgs[m_curArg], "none") == 0)
		m_separationType = BPObjectSep_None;
	else if (std::strcmp(m_ppArgs[m_curArg], "all") == 0)
		m_separationType = BPObjectSep_All;
	else if (std::strcmp(m_ppArgs[m_curArg], "shapes") == 0)
		m_separationType = BPObjectSep_Shapes;
	else if (std::strcmp(m_ppArgs[m_curArg], "joints") == 0)
		m_separationType = BPObjectSep_Joints;
	else if (std::strcmp(m_ppArgs[m_curArg], "uuid") == 0)
		m_separationType = BPObjectSep_Uuid;
	else if (std::strcmp(m_ppArgs[m_curArg], "color") == 0)
		m_separationType = BPObjectSep_Color;
	else if (std::strcmp(m_ppArgs[m_curArg], "uuidandcolor") == 0)
		m_separationType = BPObjectSep_UuidAndColor;
	else
	{
		AttachDebugConsole();
		DebugOutL("Unknown separation type: ", m_ppArgs[m_curArg]);

		return false;
	}

	m_actionFlags |= ConverterActionFlag::SepTypeSpecified;
	return true;
}

bool ArgumentParser::argCustomGameUuid()
{
	if (remainingArgs() < 1)
	{
		AttachDebugConsole();
		DebugOutL("Usage: -customgameuuid <uuid>");

		return false;
	}

	m_curArg++;

	std::string_view v_uuidString(m_ppArgs[m_curArg]);

	if (v_uuidString.size() != 36
		|| v_uuidString[8] != '-'
		|| v_uuidString[13] != '-'
		|| v_uuidString[18] != '-'
		|| v_uuidString[23] != '-')
	{
		AttachDebugConsole();
		DebugOutL("Invalid uuid format");

		return false;
	}

	m_customGameUuid = SMUuid(m_ppArgs[m_curArg]);

	m_actionFlags |= ConverterActionFlag::CustomgameUuidSpecified;
	return true;
}
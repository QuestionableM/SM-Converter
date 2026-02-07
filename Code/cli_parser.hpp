#pragma once

#include "Utils/Console.hpp"
#include "Utils/Uuid.hpp"
#include <string>

enum class ConverterType : std::uint8_t
{
	Unknown = 0,

	Blueprint = (1 << 0),
	Tile      = (1 << 1),
	World     = (1 << 2)
};

enum class ConverterActionFlag : std::uint16_t
{
	None = 0,

	DebugFlag               = (1 << 0),
	NameSpecified           = (1 << 1),
	PathSpecified           = (1 << 2),
	TypeSpecified           = (1 << 3),
	ListBlueprints          = (1 << 4),
	ListTiles               = (1 << 5),
	ListWorlds              = (1 << 6),
	ListCustomGames         = (1 << 7),
	SepTypeSpecified        = (1 << 8),
	CustomgameUuidSpecified = (1 << 9),

	List = ListBlueprints | ListTiles | ListWorlds | ListCustomGames
};

enum class ConverterExportSettings : std::uint16_t
{
	None = 0,

	// Generic settings

	ExportMaterials     = (1 << 0),
	ExportUvs           = (1 << 1),
	ExportNormals       = (1 << 2),
	DeduplicateVertices = (1 << 3),

	// Tile / World settings

	ExportClutter          = (1 << 4),
	ExportAssets           = (1 << 5),
	ExportPrefabs          = (1 << 6),
	ExportBlueprints       = (1 << 7),
	ExportKinematics       = (1 << 8),
	ExportHarvestables     = (1 << 9),
	ExportDecals           = (1 << 10),
	ExportGroundTextures   = (1 << 11),
	Export8KGroundTextures = (1 << 12)
};

template<typename T>
struct HasOperators { inline static constexpr bool Value = false; };

template<>
struct HasOperators<ConverterType> { inline static constexpr bool Value = true; };

template<>
struct HasOperators<ConverterActionFlag> { inline static constexpr bool Value = true; };

template<>
struct HasOperators<ConverterExportSettings> { inline static constexpr bool Value = true; };

template<typename T> requires(HasOperators<T>::Value)
constexpr static T operator|=(T& lhs, const T rhs) noexcept
{
	using under_type = std::underlying_type_t<T>;

	lhs = static_cast<T>(static_cast<under_type>(lhs) | static_cast<under_type>(rhs));
	return lhs;
}

template<typename T> requires(HasOperators<T>::Value)
constexpr static T operator&(const T lhs, const T rhs) noexcept
{
	using under_type = std::underlying_type_t<T>;

	return static_cast<T>(static_cast<under_type>(lhs) & static_cast<under_type>(rhs));
}

template<typename T> requires(HasOperators<T>::Value)
constexpr static T operator|(const T lhs, const T rhs) noexcept
{
	using under_type = std::underlying_type_t<T>;

	return static_cast<T>(static_cast<under_type>(lhs) | static_cast<under_type>(rhs));
}

struct ArgumentParser;

struct ArgumentParserCallback
{
	const char* argName;
	bool(ArgumentParser::*callback)();
};

struct ArgumentParser
{
	ArgumentParser(const int argc, const char* argv[]);

	bool processArgs();
	std::size_t remainingArgs();
	const ArgumentParserCallback* findCallback(const char* arg);

	bool actionFlagSet(const ConverterActionFlag flag) const noexcept;
	bool anyActionFlagSet(const ConverterActionFlag flag) const noexcept;
	bool exportFlagSet(const ConverterExportSettings flag) const noexcept;

	void applySettings() const;

	template<ConverterActionFlag t_flag>
	bool actionFlagSetter()
	{
		m_actionFlags |= t_flag;
		return true;
	}

	template<ConverterType t_type, ConverterExportSettings t_setting>
	bool exportFlagSetter()
	{
		if (!this->actionFlagSet(ConverterActionFlag::TypeSpecified))
		{
			AttachDebugConsole();
			DebugOutL(m_ppArgs[m_curArg], " requires -type to be set");

			return false;
		}

		if constexpr (t_type != ConverterType::Unknown)
		{
			if ((m_convType & t_type) == ConverterType::Unknown)
			{
				AttachDebugConsole();
				DebugOutL(m_ppArgs[m_curArg], " is unavailable for the current type");

				return false;
			}
		}

		m_exportSettings |= t_setting;
		return true;
	}

	// -type
	bool argType();
	// -path
	bool argPath();
	// -name
	bool argName();
	// -septype
	bool argSepType();
	// -customgameuuid
	bool argCustomGameUuid();

	std::size_t m_argCount;
	std::size_t m_curArg;
	const char** m_ppArgs;

	ConverterExportSettings m_exportSettings;
	ConverterActionFlag m_actionFlags;
	ConverterType m_convType;
	std::uint8_t m_separationType;
	std::wstring m_itemPath;
	std::wstring m_itemName;
	SMUuid m_customGameUuid;
};
#pragma once

#include "Converter/TileConverter/TileConverterContext.hpp"
#include "Converter/TileConverter/CellHeader.hpp"
#include "Converter/TileConverter/TilePart.hpp"
#include "Converter/ConvertSettings.hpp"

#include "ObjectDatabase/Mods/Mod.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/Lz4Lib.hpp"
#include "Utils/Uuid.hpp"

SM_UNMANAGED_CODE

class ClutterReader
{
	ClutterReader() = default;

public:
	static void Read(TileReaderContext& context);
	static bool Read(TileReaderContext& context, std::vector<Byte>& outBytes);

	static void Read(const std::vector<Byte>& bytes, TilePart* pPart);
};

SM_MANAGED_CODE
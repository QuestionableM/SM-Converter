#pragma once

#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

#include <cstdint>

struct TileReaderContext
{
	const class CellHeader* pCellHeader;
	class MemoryWrapper* pReader;
	class ConvertError* pError;
	class TilePart* pTilePart;
	std::uint32_t tileVersion;
	bool counterMode;
};

struct PrefabReaderContext
{
	class MemoryWrapper* pReader;
	class BitStream* pBitStream;
	class ConvertError* pError;
	class SMPrefab* pPrefab;
	std::uint32_t prefabVersion;
	bool counterMode;
};

SM_MANAGED_CODE
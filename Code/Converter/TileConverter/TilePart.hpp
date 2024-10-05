#pragma once

#include "Converter/Entity/TileClutter.hpp"
#include "Converter/Entity/Harvestable.hpp"
#include "Converter/Entity/Blueprint.hpp"
#include "Converter/Entity/Prefab.hpp"
#include "Converter/Entity/Asset.hpp"
#include "Converter/WriterOffset.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/ByteImpl.hpp"

SM_UNMANAGED_CODE

class Tile;

class TilePart
{
public:
	TilePart();
	~TilePart();

	void AddObject(SMEntity* object, int index = 0);

	void WriteToFile(
		std::ofstream& model,
		WriterOffsetData& offset,
		int tileWidth,
		int tileHeight,
		int xPos,
		int zPos);

	void WriteToFileWorld(
		std::ofstream& model,
		WriterOffsetData& offset,
		std::size_t posX,
		std::size_t posY,
		std::size_t worldSz,
		char rotation);

	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tData) const;
	std::size_t GetAmountOfObjects() const;

public:
	// Mip
	std::array<float, 33 * 33> m_vertexHeight;
	std::array<int, 33 * 33> m_vertexColor;
	std::array<long long, 65 * 65> m_ground;

	// Clutter
	std::array<SignedByte, 128 * 128> m_clutter;
	std::array<SMTileClutter*, 128 * 128> m_clutterMap;

	/*
		the 1st vector contains: harvestables, assets, prefabs, decals and blueprints
		the 2nd-4th vectors contain: assets and harvestables
	*/
	std::array<std::vector<SMEntity*>, 4> m_objects = {};
};

SM_MANAGED_CODE
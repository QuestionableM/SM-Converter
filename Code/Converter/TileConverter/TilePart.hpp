#pragma once

#include "Converter\Entity\TileClutter.hpp"
#include "Converter\Entity\Harvestable.hpp"
#include "Converter\Entity\Blueprint.hpp"
#include "Converter\Entity\Prefab.hpp"
#include "Converter\Entity\Asset.hpp"
#include "Converter\WriterOffset.hpp"

#include "Utils\ByteImpl.hpp"

#pragma unmanaged

class Tile;

class TilePart
{
	Tile* Parent;

public:
	// Mip
	std::array<float, 33 * 33> m_VertexHeight;
	std::array<int, 33 * 33> m_VertexColor;
	std::array<long long, 65 * 65> m_Ground;

	// Clutter
	std::array<SignedByte, 128 * 128> m_Clutter;
	std::array<SMTileClutter*, 128 * 128> m_ClutterMap;

	/*
		the 1st vector contains: harvestables, assets, prefabs, decals and blueprints
		the 2nd-4th vectors contain: assets and harvestables
	*/
	std::array<std::vector<SMEntity*>, 4> m_Objects = {};

public:
	TilePart(Tile* parent);
	~TilePart();

	void AddObject(SMEntity* object, const int& index = 0);

	inline Tile* GetParent() { return this->Parent; }

	void WriteToFile(std::ofstream& model, WriterOffsetData& mOffsetData, const int& xPos, const int& zPos);
	void WriteToFileWorld(std::ofstream& v_model, WriterOffsetData& v_offset, const std::size_t& x_pos, const std::size_t& y_pos, const std::size_t& v_world_sz, const char& v_rotation);

	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tData) const;
	std::size_t GetAmountOfObjects() const;
};

#pragma managed
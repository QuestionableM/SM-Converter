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

	void AddObject(SMEntity* object, int index = 0);

	inline Tile* GetParent() noexcept { return this->Parent; }

	void WriteToFile(std::ofstream& model, WriterOffsetData& mOffsetData, int xPos, int zPos);
	void WriteToFileWorld(std::ofstream& v_model, WriterOffsetData& v_offset, std::size_t x_pos, std::size_t y_pos, std::size_t v_world_sz, char v_rotation);

	void FillTextureMap(std::unordered_map<std::string, ObjectTexData>& tData) const;
	std::size_t GetAmountOfObjects() const;
};

SM_MANAGED_CODE
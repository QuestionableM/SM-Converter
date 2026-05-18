#pragma once

#include "Converter/TileConverter/Readers/TileReader.hpp"
#include "Converter/TileConverter/Tile.hpp"
#include "ObjectDatabase/ProgCounter.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

struct WorldCellData
{
	TilePart* part = nullptr;
	char rotation = 0;
};

class SMWorld
{
public:
	SMWorld(std::size_t cellCount, const std::size_t width);

	static SMWorld* LoadFromFile(const std::wstring& path, ConvertError& error, const bool counterMode);

	Tile* ReadTile(const std::wstring& path, const bool counterMode);
	void LoadCell(const simdjson::dom::element& v_cell, const bool counterMode);

	void SetCell(const std::size_t x, const std::size_t y, TilePart* pCell, const char rotation);
	const WorldCellData& GetCell(const std::size_t x, const std::size_t y) const;

	void GetVertexHeight(std::vector<float>& v_vert_array) const;
	void GetClutterData(std::vector<SMTileClutter*>& v_clutter_array) const;

	void GenerateTerrainMesh(Model& outModel, const std::vector<float>& vertArray) const;
	void WriteTerrain(std::ofstream& model, WriterOffsetData& v_offset, const std::vector<float>& height_map) const;
	void WriteClutter(std::ofstream& model, WriterOffsetData& v_offset, const std::vector<float>& height_map) const;

	std::size_t	GetAmountOfObjects() const;
	void WriteAssets(std::ofstream& model, WriterOffsetData& v_offset) const;

	void WriteMtlFile(const std::wstring& path) const;
	bool WriteObjFile(const std::wstring& dir_path, const std::wstring& file_name, const std::wstring& mtl_name) const;

	void WriteToFile(const std::wstring& dir_path, const std::wstring& file_name) const;

	inline std::size_t GetWidth() const noexcept { return m_width; }

private:
	std::size_t m_width;

	//Stores unique tiles
	std::unordered_map<std::wstring, Tile> m_tileMap = {};

	//A vector that stores the positions of cells in the world
	std::vector<WorldCellData> m_cellMap = {};
};

SM_MANAGED_CODE
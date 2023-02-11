#pragma once

#include "Converter\TileConverter\Tile.hpp"

#pragma unmanaged

struct WorldCellData
{
	TilePart* part = nullptr;
	char rotation = 0;
};

class SMWorld
{
public:
	inline SMWorld(const std::size_t& v_cell_count, const std::size_t& v_width)
	{
		m_cellMap.resize(v_cell_count);
		m_width = v_width;
	}

	inline ~SMWorld()
	{
		for (auto& v_cur_tile : m_tileMap)
			delete v_cur_tile.second;
	}

	Tile* ReadTile(const std::wstring& path);

	inline void SetCell(const std::size_t& x, const std::size_t& y, TilePart* v_cell, const char& v_rotation)
	{
		if (x >= m_width || y >= m_width)
		{
			DebugErrorL("Can't set the cell outside of bounds: (Position: ", x, ", ", y, "; Bounds: ", m_width, ", ", m_width, ")");
			return;
		}

		m_cellMap[x + y * m_width] = { v_cell, v_rotation };
	}

	inline const WorldCellData& GetCell(const std::size_t& x, const std::size_t& y) const
	{
		return m_cellMap[x + y * m_width];
	}

	void LoadCell(const simdjson::dom::element& v_cell);
	static SMWorld* LoadFromFile(const std::wstring& path, ConvertError& v_error);

	void GetVertexHeight(std::vector<float>& v_vert_array) const;
	void GetClutterData(std::vector<SMTileClutter*>& v_clutter_array) const;

	Model* GenerateTerrainMesh(const std::vector<float>& v_vert_array) const;
	void WriteTerrain(std::ofstream& model, WriterOffsetData& v_offset, const std::vector<float>& height_map) const;
	void WriteClutter(std::ofstream& model, WriterOffsetData& v_offset, const std::vector<float>& height_map) const;

	std::size_t	GetAmountOfObjects() const;
	void WriteAssets(std::ofstream& model, WriterOffsetData& v_offset) const;

	void WriteMtlFile(const std::wstring& path) const;
	bool WriteObjFile(const std::wstring& dir_path, const std::wstring& file_name, const std::wstring& mtl_name) const;

	void WriteToFile(const std::wstring& dir_path, const std::wstring& file_name) const;

private:
	std::size_t m_width;

	//Stores unique tiles
	std::unordered_map<std::wstring, Tile*> m_tileMap = {};

	//A vector that stores the positions of cells in the world
	std::vector<WorldCellData> m_cellMap = {};
};

#pragma managed
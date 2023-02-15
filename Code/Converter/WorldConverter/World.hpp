#pragma once

#include "Converter\TileConverter\Readers\TileReader.hpp"
#include "Converter\TileConverter\Tile.hpp"
#include "ObjectDatabase\ProgCounter.hpp"

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

	template<bool t_mod_counter>
	Tile* ReadTile(const std::wstring& path)
	{
		if (path.empty())
			return nullptr;

		if (!File::Exists(path))
		{
			DebugErrorL("Tile doesn't exist: ", path);
			return nullptr;
		}

		std::wstring v_full_path;
		if (!File::GetFullFilePath(path, v_full_path))
		{
			DebugErrorL("Couldn't get the full path from the specified path: ", path);
			return nullptr;
		}

		const auto v_iter = m_tileMap.find(v_full_path);
		if (v_iter != m_tileMap.end())
			return v_iter->second;

		ConvertError v_error;
		Tile* v_new_tile = TileReader::ReadTile<t_mod_counter>(v_full_path, v_error);
		if (!v_new_tile)
		{
			DebugErrorL("Couldn't read the specified tile: ", v_full_path, "\nError: UNKNOWN");
			return nullptr;
		}

		if (v_error)
		{
			if (v_new_tile) delete v_new_tile;

			DebugErrorL("Couldn't read the specified tile: ", v_full_path, "\nError: ", v_error.GetErrorMsg());
			return nullptr;
		}

		m_tileMap.insert(std::make_pair(v_full_path, v_new_tile));
		DebugOutL("Reading a new tile: ", v_full_path);
		return v_new_tile;
	}

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

	template<bool t_mod_counter>
	inline void LoadCell(const simdjson::dom::element& v_cell)
	{
		if (!v_cell.is_object()) return;

		const auto v_offset_x = v_cell["offsetX"];
		const auto v_offset_y = v_cell["offsetY"];
		const auto v_path = v_cell["path"];
		const auto v_rotation = v_cell["rotation"];
		const auto v_pos_x = v_cell["x"];
		const auto v_pos_y = v_cell["y"];

		if (!(v_path.is_string() && v_rotation.is_number()))
			return;

		if (!(v_offset_x.is_number() && v_offset_y.is_number()))
			return;

		if (!(v_pos_x.is_number() && v_pos_y.is_number()))
			return;

		std::wstring v_tile_path = String::ToWide(v_path.get_string());
		KeywordReplacer::ReplaceKeyR(v_tile_path);

		Tile* v_tile = this->ReadTile<t_mod_counter>(v_tile_path);
		if (!v_tile) return;

		if constexpr (!t_mod_counter)
		{
			const int v_cell_x = JsonReader::GetNumber<int>(v_offset_x);
			const int v_cell_y = JsonReader::GetNumber<int>(v_offset_y);
			TilePart* v_cur_cell = v_tile->GetPartSafe(v_cell_x, v_cell_y);

			const int v_half_width = static_cast<int>(m_width) / 2;
			const int v_world_pos_x = JsonReader::GetNumber<int>(v_pos_x) + v_half_width;
			const int v_world_pos_y = JsonReader::GetNumber<int>(v_pos_y) + v_half_width;
			const char v_rotation_idx = JsonReader::GetNumber<char>(v_rotation);
			this->SetCell(v_world_pos_x, v_world_pos_y, v_cur_cell, v_rotation_idx);
		}
	}

	template<bool t_mod_counter>
	inline static SMWorld* LoadFromFile(const std::wstring& path, ConvertError& v_error)
	{
		simdjson::dom::document v_doc;
		if (!JsonReader::LoadParseSimdjsonCommentsC(path, v_doc, simdjson::dom::element_type::OBJECT))
		{
			v_error = ConvertError(1, L"Couldn't read the specified file. Possible reasons: Invalid Json File, Invalid Path, Syntax Error");
			return nullptr;
		}

		const auto v_doc_root = v_doc.root();

		const auto v_cell_data = v_doc_root["cellData"];
		if (!v_cell_data.is_array())
		{
			v_error = ConvertError(1, L"Couldn't find a cell data inside the world file");
			return nullptr;
		}

		const auto v_cell_data_array = v_cell_data.get_array();

		//New worlds have `version` property
		const auto v_world_version = v_doc_root["version"];

		std::size_t v_cell_count = 484, v_world_width = 22;
		if (v_world_version.is_number())
		{
			v_cell_count = v_cell_data_array.size();
			v_world_width = static_cast<int>(std::sqrtf(static_cast<float>(v_cell_count)));
		}

		const int v_half_width = static_cast<int>(v_world_width / 2);
		SMWorld* v_world = new SMWorld(v_cell_count, v_world_width);

		DebugOutL("Amount of cells: ", v_world->m_cellMap.size());
		if constexpr (!t_mod_counter) {
			ProgCounter::SetState(ProgState::ReadingCells, v_world->m_cellMap.size());
		}

		for (const auto v_cell : v_cell_data_array)
		{
			v_world->LoadCell<t_mod_counter>(v_cell);

			if constexpr (!t_mod_counter) {
				ProgCounter::ProgressValue++;
			}
		}

		return v_world;
	}

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

	inline const std::size_t& GetWidth() const { return m_width; }

private:
	std::size_t m_width;

	//Stores unique tiles
	std::unordered_map<std::wstring, Tile*> m_tileMap = {};

	//A vector that stores the positions of cells in the world
	std::vector<WorldCellData> m_cellMap = {};
};

#pragma managed
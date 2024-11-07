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
	inline SMWorld(std::size_t v_cell_count, std::size_t v_width) :
		m_width(v_width),
		m_tileMap(),
		m_cellMap(v_cell_count)
	{}

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

		std::wstring v_fullPath;
		if (!File::GetFullFilePath(path, v_fullPath))
		{
			DebugErrorL("Couldn't get the full path from the specified path: ", path);
			return nullptr;
		}

		const auto v_iter = m_tileMap.find(v_fullPath);
		if (v_iter != m_tileMap.end())
			return &v_iter->second;

		ConvertError v_error;
		Tile v_newTile;
		if (!TileReader::ReadTile<t_mod_counter>(v_fullPath, v_error, v_newTile))
		{
			DebugErrorL("Couldn't read the specified tile: ", v_fullPath, "\nError: UNKNOWN");
			return nullptr;
		}

		if (v_error)
		{
			DebugErrorL("Couldn't read the specified tile: ", v_fullPath, "\nError: ", v_error.getErrorMsg());
			return nullptr;
		}

		DebugOutL("Reading a new tile: ", v_fullPath);

		return &m_tileMap.emplace(std::move(v_fullPath), std::move(v_newTile)).first->second;
	}

	inline void SetCell(std::size_t x, std::size_t y, TilePart* v_cell, char v_rotation)
	{
		if (x >= m_width || y >= m_width)
		{
			DebugErrorL("Can't set the cell outside of bounds: (Position: ", x, ", ", y, "; Bounds: ", m_width, ", ", m_width, ")");
			return;
		}

		m_cellMap[x + y * m_width] = { v_cell, v_rotation };
	}

	inline const WorldCellData& GetCell(std::size_t x, std::size_t y) const
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

		std::wstring v_tilePath = String::ToWide(v_path.get_string().value_unsafe());
		KeywordReplacer::ReplaceKeyR(v_tilePath);

		Tile* v_pTile = this->ReadTile<t_mod_counter>(v_tilePath);
		if (!v_pTile) return;

		if constexpr (!t_mod_counter)
		{
			const int v_cellX = JsonReader::GetNumber<int>(v_offset_x.value_unsafe());
			const int v_cellY = JsonReader::GetNumber<int>(v_offset_y.value_unsafe());
			TilePart* v_pCurCell = v_pTile->GetPartSafe(v_cellX, v_cellY);

			const int v_halfWidth = static_cast<int>(m_width) / 2;
			const int v_worldPosX = JsonReader::GetNumber<int>(v_pos_x.value_unsafe()) + v_halfWidth;
			const int v_worldPosY = JsonReader::GetNumber<int>(v_pos_y.value_unsafe()) + v_halfWidth;
			const char v_rotationIdx = JsonReader::GetNumber<char>(v_rotation.value_unsafe());
			this->SetCell(v_worldPosX, v_worldPosY, v_pCurCell, v_rotationIdx);
		}
	}

	template<bool t_mod_counter>
	inline static SMWorld* LoadFromFile(const std::wstring& path, ConvertError& error)
	{
		simdjson::dom::document v_doc;
		if (!JsonReader::LoadParseSimdjsonCommentsC(path, v_doc, simdjson::dom::element_type::OBJECT))
		{
			error.setError(1, "Couldn't read the specified file. Possible reasons: Invalid Json File, Invalid Path, Syntax Error");
			return nullptr;
		}

		const auto v_doc_root = v_doc.root();

		const auto v_cell_data = v_doc_root["cellData"];
		if (!v_cell_data.is_array())
		{
			error.setError(1, "Couldn't find a cell data inside the world file");
			return nullptr;
		}

		const auto v_cell_data_array = v_cell_data.get_array().value_unsafe();

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
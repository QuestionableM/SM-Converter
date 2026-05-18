#include "World.hpp"

#include "Converter/Entity/GroundTerrainData.hpp"
#include "Converter/MtlFileWriter.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/File.hpp"
#include "Utils/Json.hpp"
#include "Utils/Math.hpp"

SM_UNMANAGED_CODE

#include <PerlinNoise/PerlinNoise.hpp>
#include <FreeImage.h>

SMWorld::SMWorld(
	const std::size_t cellCount,
	const std::size_t width
)
	: m_width(width)
	, m_tileMap()
	, m_cellMap(cellCount)
{}

SMWorld* SMWorld::LoadFromFile(
	const std::wstring& path,
	ConvertError& error,
	const bool counterMode)
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
	if (!counterMode)
		ProgCounter::SetState(ProgState::ReadingCells, v_world->m_cellMap.size());

	for (const auto v_cell : v_cell_data_array)
	{
		v_world->LoadCell(v_cell, counterMode);

		if (!counterMode)
			ProgCounter::ProgressValue++;
	}

	return v_world;
}

Tile* SMWorld::ReadTile(
	const std::wstring& path,
	const bool counterMode)
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
	if (!TileReader::ReadTile(v_fullPath, v_error, v_newTile, counterMode))
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

void SMWorld::LoadCell(
	const simdjson::dom::element& v_cell,
	const bool counterMode)
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

	Tile* v_pTile = this->ReadTile(v_tilePath, counterMode);
	if (!v_pTile) return;

	if (!counterMode)
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

void SMWorld::SetCell(
	const std::size_t x,
	const std::size_t y,
	TilePart* pCell,
	const char rotation)
{
	if (x >= m_width || y >= m_width)
	{
		DebugErrorL("Can't set the cell outside of bounds: (Position: ", x, ", ", y, "; Bounds: ", m_width, ", ", m_width, ")");
		return;
	}

	m_cellMap[x + y * m_width] = { pCell, rotation };
}

const WorldCellData& SMWorld::GetCell(
	const std::size_t x,
	const std::size_t y) const
{
	return m_cellMap[x + y * m_width];
}

void SMWorld::GetVertexHeight(std::vector<float>& v_vert_array) const
{
	const std::size_t v_world_sz = m_width * 32 + 1;
	v_vert_array.resize(v_world_sz * v_world_sz);

	for (std::size_t y = 0; y < m_width; y++)
	{
		for (std::size_t x = 0; x < m_width; x++)
		{
			const WorldCellData& v_celldata = this->GetCell(x, y);
			TilePart* v_part = v_celldata.part;

			const std::size_t v_idx = x * 32 + y * 32 * v_world_sz;
			if (v_part)
			{
				switch (v_celldata.rotation)
				{
				case 1:
					{
						for (std::size_t a = 0; a < 33; a++)
							for (std::size_t b = 0; b < 33; b++)
								v_vert_array[v_idx + (32 - b) + a * v_world_sz] = v_part->m_vertexHeight[a + b * 33];

						break;
					}
				case 2:
					{
						for (std::size_t a = 0; a < 33; a++)
							for (std::size_t b = 0; b < 33; b++)
								v_vert_array[v_idx + (32 - b) + (32 - a) * v_world_sz] = v_part->m_vertexHeight[b + a * 33];

						break;
					}
				case 3:
					{
						for (std::size_t a = 0; a < 33; a++)
							for (std::size_t b = 0; b < 33; b++)
								v_vert_array[v_idx + b + (32 - a) * v_world_sz] = v_part->m_vertexHeight[a + b * 33];

						break;
					}
				default:
					{
						for (std::size_t a = 0; a < 33; a++)
							std::memcpy(v_vert_array.data() + (v_idx + a * v_world_sz), v_part->m_vertexHeight.data() + (a * 33), 33 * sizeof(float));

						break;
					}
				}
			}
			else
			{
				for (std::size_t a = 0; a < 33; a++)
					std::memset(v_vert_array.data() + (v_idx + a * v_world_sz), 0, 33 * sizeof(float));
			}
		}
	}
}

void SMWorld::GetClutterData(std::vector<SMTileClutter*>& v_clutter_array) const
{
	const std::size_t v_world_sz = m_width * 128;
	v_clutter_array.resize(v_world_sz * v_world_sz);

	for (std::size_t y = 0; y < m_width; y++)
	{
		for (std::size_t x = 0; x < m_width; x++)
		{
			const WorldCellData& v_celldata = this->GetCell(x, y);
			TilePart* v_part = v_celldata.part;

			const std::size_t v_idx = x * 128 + y * 128 * v_world_sz;
			if (v_part)
			{
				switch (v_celldata.rotation)
				{
				case 1:
				{
					for (std::size_t a = 0; a < 128; a++)
						for (std::size_t b = 0; b < 128; b++)
							v_clutter_array[v_idx + (127 - b) + a * v_world_sz] = v_part->m_clutterMap[a + b * 128];
					
					break;
				}
				case 2:
					{
						for (std::size_t a = 0; a < 128; a++)
							for (std::size_t b = 0; b < 128; b++)
								v_clutter_array[v_idx + (127 - b) + (127 - a) * v_world_sz] = v_part->m_clutterMap[b + a * 128];

						break;
					}
				case 3:
					{
						for (std::size_t a = 0; a < 128; a++)
							for (std::size_t b = 0; b < 128; b++)
								v_clutter_array[v_idx + b + (127 - a) * v_world_sz] = v_part->m_clutterMap[a + b * 128];

						break;
					}
				default:
					{
						for (std::size_t a = 0; a < 128; a++)
							std::memcpy(v_clutter_array.data() + (v_idx + a * v_world_sz), v_part->m_clutterMap.data() + (a * 128), 128 * sizeof(std::size_t));

						break;
					}
				}
			}
			else
			{
				for (std::size_t a = 0; a < 128; a++)
					std::memset(v_clutter_array.data() + (v_idx + a * v_world_sz), 0, 128 * sizeof(std::size_t));
			}
		}
	}
}

inline glm::vec3 CalculateNormalVector(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
	//p2 is the new origin (can be changed later)
	const glm::vec3 a = p2 - p1;
	const glm::vec3 b = p3 - p1;

	return glm::normalize(glm::cross(a, b));
}

void SMWorld::GenerateTerrainMesh(Model& outModel, const std::vector<float>& vertArray) const
{
	DebugOutL("Generating terrain mesh...");

	const std::size_t v_world_size = m_width * 32 + 1;
	const std::size_t v_world_size_sq = v_world_size * v_world_size;

	const float vf_world_size = static_cast<float>(m_width) * 32.0f;

	std::vector<std::size_t> normal_div = {};
	if (SharedConverterSettings::ExportNormals)
	{
		outModel.m_normals.reserve(v_world_size_sq);
		normal_div.reserve(v_world_size_sq);
	}

	outModel.m_vertices.reserve(v_world_size_sq);
	for (std::size_t y = 0; y < v_world_size; y++)
	{
		for (std::size_t x = 0; x < v_world_size; x++)
		{
			const float& height = vertArray[x + y * v_world_size];
			const float vert_x = -(static_cast<float>(x) * 2.0f) + vf_world_size;
			const float vert_y = -(static_cast<float>(y) * 2.0f) + vf_world_size;

			outModel.m_vertices.emplace_back(vert_x, vert_y, height);

			if (SharedConverterSettings::ExportNormals)
			{
				outModel.m_normals.emplace_back(0.0f);
				normal_div.push_back(1);
			}
		}
	}

	if (SharedConverterSettings::ExportUvs)
	{
		const float v_uv_size = static_cast<float>(v_world_size - 1);

		outModel.m_uvs.reserve(v_world_size_sq);
		for (std::size_t y = 0; y < v_world_size; y++)
		{
			for (std::size_t x = 0; x < v_world_size; x++)
			{
				const float u = static_cast<float>(x) / v_uv_size;
				const float v = static_cast<float>(y) / v_uv_size;

				outModel.m_uvs.emplace_back(u, v);
			}
		}
	}

	SubMeshData& v_newSubMesh = outModel.m_subMeshData.emplace_back(
		0,
		SharedConverterSettings::ExportNormals,
		SharedConverterSettings::ExportUvs
	);

	//generate normals
	v_newSubMesh.m_dataIdx.reserve(v_world_size_sq);
	for (std::size_t y = 0; y < v_world_size - 1; y++)
	{
		for (std::size_t x = 0; x < v_world_size - 1; x++)
		{
			const std::size_t h00 = (x    ) + (y    ) * v_world_size;
			const std::size_t h01 = (x    ) + (y + 1) * v_world_size;
			const std::size_t h10 = (x + 1) + (y    ) * v_world_size;
			const std::size_t h11 = (x + 1) + (y + 1) * v_world_size;

			if (SharedConverterSettings::ExportNormals)
			{
				const glm::vec3& p1 = outModel.m_vertices[h00];
				const glm::vec3& p2 = outModel.m_vertices[h01];
				const glm::vec3& p3 = outModel.m_vertices[h10];
				const glm::vec3& p4 = outModel.m_vertices[h11];

				const glm::vec3 t1_norm = CalculateNormalVector(p1, p3, p2); //first_triangle
				const glm::vec3 t2_norm = CalculateNormalVector(p2, p3, p4); //second_triangle

				outModel.m_normals[h00] += t1_norm;
				outModel.m_normals[h01] += (t1_norm + t2_norm);
				outModel.m_normals[h10] += (t1_norm + t2_norm);
				outModel.m_normals[h11] += t2_norm;

				normal_div[h00] += 1;
				normal_div[h01] += 2;
				normal_div[h10] += 2;
				normal_div[h11] += 1;
			}

			const VertexData vert1 = { h00, SharedConverterSettings::ExportUvs ? h00 : 0, SharedConverterSettings::ExportNormals ? h00 : 0 };
			const VertexData vert2 = { h01, SharedConverterSettings::ExportUvs ? h01 : 0, SharedConverterSettings::ExportNormals ? h01 : 0 };
			const VertexData vert3 = { h10, SharedConverterSettings::ExportUvs ? h10 : 0, SharedConverterSettings::ExportNormals ? h10 : 0 };
			const VertexData vert4 = { h11, SharedConverterSettings::ExportUvs ? h11 : 0, SharedConverterSettings::ExportNormals ? h11 : 0 };

			v_newSubMesh.m_dataIdx.emplace_back(std::initializer_list{ vert1, vert3, vert2 });
			v_newSubMesh.m_dataIdx.emplace_back(std::initializer_list{ vert2, vert3, vert4 });
		}
	}

	if (SharedConverterSettings::ExportNormals)
	{
		//make normals smooth
		for (std::size_t y = 0; y < v_world_size - 1; y++)
		{
			for (std::size_t x = 0; x < v_world_size - 1; x++)
			{
				const std::size_t h00 = (x    ) + (y    ) * v_world_size;
				const std::size_t h01 = (x    ) + (y + 1) * v_world_size;
				const std::size_t h10 = (x + 1) + (y    ) * v_world_size;
				const std::size_t h11 = (x + 1) + (y + 1) * v_world_size;

				outModel.m_normals[h00] = glm::normalize(outModel.m_normals[h00] / static_cast<float>(normal_div[h00]));
				outModel.m_normals[h01] = glm::normalize(outModel.m_normals[h01] / static_cast<float>(normal_div[h01]));
				outModel.m_normals[h10] = glm::normalize(outModel.m_normals[h10] / static_cast<float>(normal_div[h10]));
				outModel.m_normals[h11] = glm::normalize(outModel.m_normals[h11] / static_cast<float>(normal_div[h11]));
			}
		}
	}
}

void SMWorld::WriteTerrain(std::ofstream& model, WriterOffsetData& v_offset, const std::vector<float>& height_map) const
{
	DebugOutL("Writing terrain...");
	ProgCounter::SetState(ProgState::WritingGroundMesh, 0);

	//Used to generate the mamterial for ground terrain mesh
	SMGroundTerrainData v_tmpTerrainData;
	Model v_terrainSurface;
	this->GenerateTerrainMesh(v_terrainSurface, height_map);

	const glm::mat4 v_rotation = glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
	v_terrainSurface.WriteToFile(v_rotation, v_offset, model, &v_tmpTerrainData);
}

void SMWorld::WriteClutter(std::ofstream& model, WriterOffsetData& v_offset, const std::vector<float>& height_map) const
{
	if (!TileConverterSettings::ExportClutter) return;

	DebugOutL("Writing clutter...");

	std::vector<SMTileClutter*> tile_clutter;
	this->GetClutterData(tile_clutter);

	const std::size_t v_world_sz = m_width * 128;
	const float v_world_sz_f = static_cast<float>(v_world_sz);

	const std::size_t v_grid_sz = m_width * 32;
	const float v_grid_sz_f = static_cast<float>(v_grid_sz);

	//initialize perlin noise
	const siv::PerlinNoise rotation_noise(1337u);
	const siv::PerlinNoise scale_noise(1488u);
	const siv::PerlinNoise x_noise(6842u);
	const siv::PerlinNoise y_noise(1813u);

	ProgCounter::SetState(ProgState::WritingClutter, v_world_sz * v_world_sz);
	for (std::size_t y = 0; y < v_world_sz; y++)
	{
		for (std::size_t x = 0; x < v_world_sz; x++)
		{
			SMTileClutter* v_cur_clutter = tile_clutter[x + y * v_world_sz];
			if (!v_cur_clutter)
			{
				ProgCounter::ProgressValue++;
				continue;
			}

			const float x_offset = (float)x_noise.octave2D_11((double)x * 91.42f, (double)y * 83.24f, 4) * 0.5f;
			const float y_offset = (float)y_noise.octave2D_11((double)x * 73.91f, (double)y * 98.46f, 4) * 0.5f;

			const float v_x_clamp = std::clamp<float>((float)x + x_offset, 0.0f, v_world_sz_f);
			const float v_y_clamp = std::clamp<float>((float)y + y_offset, 0.0f, v_world_sz_f);

			const glm::vec3 v_clutter_pos(
				-((float)(v_world_sz - x - 1) * 0.5f) + v_grid_sz_f + x_offset,
				-((float)(v_world_sz - y - 1) * 0.5f) + v_grid_sz_f + y_offset,
				Math::GetHeightPointBox(height_map, v_world_sz, v_grid_sz, v_x_clamp, v_y_clamp)
			);

			const float rot_angle = (float)rotation_noise.octave2D_11((double)x * 15.0, (double)y * 17.14, 4) * glm::two_pi<float>();
			const float rand_scale = (float)scale_noise.octave2D_11((double)x * 54.4f, (double)y * 24.54, 8) * v_cur_clutter->ScaleVariance();

			v_cur_clutter->SetPosition(v_clutter_pos);
			v_cur_clutter->SetRotation(glm::rotate(glm::quat(), rot_angle, glm::vec3(0.0f, 0.0f, 1.0f)));
			v_cur_clutter->SetSize(glm::vec3(0.25f - (rand_scale * 0.25f)));
			v_cur_clutter->WriteObjectToFile(model, v_offset, glm::mat4(1.0f));

			ProgCounter::ProgressValue++;
		}
	}
}

std::size_t	SMWorld::GetAmountOfObjects() const
{
	std::size_t v_output = 0;

	for (const WorldCellData& v_part : m_cellMap)
	{
		if (!v_part.part) continue;
		v_output += v_part.part->GetAmountOfObjects();
	}

	return v_output;
}

void SMWorld::WriteAssets(std::ofstream& model, WriterOffsetData& v_offset) const
{
	DebugOutL("Writing assets...");

	ProgCounter::SetState(ProgState::WritingObjects, this->GetAmountOfObjects());
	for (std::size_t y = 0; y < m_width; y++)
	{
		for (std::size_t x = 0; x < m_width; x++)
		{
			const WorldCellData& v_celldata = this->GetCell(x, y);
			if (!v_celldata.part) continue;

			v_celldata.part->WriteToFileWorld(model, v_offset, x, y, m_width, v_celldata.rotation);
		}
	}
}

void SMWorld::WriteMtlFile(const std::wstring& path) const
{
	if (!SharedConverterSettings::ExportMaterials) return;
	DebugOutL("Writing an mtl file...");

	ProgCounter::SetState(ProgState::WritingMtlFile, 0);

	SMEntity::EntityTextureMap v_textureMap;
	for (const WorldCellData& v_cell : m_cellMap)
		if (v_cell.part) v_cell.part->FillTextureMap(v_textureMap);

	ProgCounter::ProgressMax = v_textureMap.size();

	MtlFileWriter::AddGroundTextures(v_textureMap);
	MtlFileWriter::Write(path, v_textureMap);
}

bool SMWorld::WriteObjFile(const std::wstring& dir_path, const std::wstring& file_name, const std::wstring& mtl_name) const
{
	std::ofstream v_outputModel(dir_path + file_name + L".obj");
	if (!v_outputModel.is_open())
	{
		DebugOutL("Fail!");
		return false;
	}

	if (SharedConverterSettings::ExportMaterials)
	{
		const std::string v_mtlHeader = "mtllib " + String::ToUtf8(mtl_name) + "\n";
		v_outputModel.write(v_mtlHeader.c_str(), v_mtlHeader.size());
	}

	WriterOffsetData v_offsetData;

	{
		std::vector<float> v_heightArray;
		this->GetVertexHeight(v_heightArray);

		this->WriteTerrain(v_outputModel, v_offsetData, v_heightArray);
		this->WriteClutter(v_outputModel, v_offsetData, v_heightArray);

		ProgCounter::SetState(ProgState::MemoryCleanup, 0);
	}

	this->WriteAssets(v_outputModel, v_offsetData);
	ProgCounter::SetState(ProgState::MemoryCleanup, 0);

	return true;
}

void SMWorld::WriteToFile(const std::wstring& dir_path, const std::wstring& file_name) const
{
	const std::wstring v_mtl_name = file_name + L".mtl";
	const std::wstring v_mtl_path = dir_path + v_mtl_name;

	if (!this->WriteObjFile(dir_path, file_name, v_mtl_name))
		return;

	this->WriteMtlFile(v_mtl_path);
}
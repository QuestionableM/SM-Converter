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
								v_vert_array[v_idx + (32 - b) + a * v_world_sz] = v_part->m_VertexHeight[a + b * 33];

						break;
					}
				case 2:
					{
						for (std::size_t a = 0; a < 33; a++)
							for (std::size_t b = 0; b < 33; b++)
								v_vert_array[v_idx + (32 - b) + (32 - a) * v_world_sz] = v_part->m_VertexHeight[b + a * 33];

						break;
					}
				case 3:
					{
						for (std::size_t a = 0; a < 33; a++)
							for (std::size_t b = 0; b < 33; b++)
								v_vert_array[v_idx + b + (32 - a) * v_world_sz] = v_part->m_VertexHeight[a + b * 33];

						break;
					}
				default:
					{
						for (std::size_t a = 0; a < 33; a++)
							std::memcpy(v_vert_array.data() + (v_idx + a * v_world_sz), v_part->m_VertexHeight.data() + (a * 33), 33 * sizeof(float));

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
							v_clutter_array[v_idx + (127 - b) + a * v_world_sz] = v_part->m_ClutterMap[a + b * 128];
					
					break;
				}
				case 2:
					{
						for (std::size_t a = 0; a < 128; a++)
							for (std::size_t b = 0; b < 128; b++)
								v_clutter_array[v_idx + (127 - b) + (127 - a) * v_world_sz] = v_part->m_ClutterMap[b + a * 128];

						break;
					}
				case 3:
					{
						for (std::size_t a = 0; a < 128; a++)
							for (std::size_t b = 0; b < 128; b++)
								v_clutter_array[v_idx + b + (127 - a) * v_world_sz] = v_part->m_ClutterMap[a + b * 128];

						break;
					}
				default:
					{
						for (std::size_t a = 0; a < 128; a++)
							std::memcpy(v_clutter_array.data() + (v_idx + a * v_world_sz), v_part->m_ClutterMap.data() + (a * 128), 128 * sizeof(std::size_t));

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

Model* SMWorld::GenerateTerrainMesh(const std::vector<float>& v_vert_array) const
{
	DebugOutL("Generating terrain mesh...");

	Model* tMesh = new Model();

	const std::size_t v_world_size = m_width * 32 + 1;
	const std::size_t v_world_size_sq = v_world_size * v_world_size;

	const float vf_world_size = static_cast<float>(m_width) * 32.0f;

	std::vector<std::size_t> normal_div = {};
	if (SharedConverterSettings::ExportNormals)
	{
		tMesh->normals.reserve(v_world_size_sq);
		normal_div.reserve(v_world_size_sq);
	}

	tMesh->vertices.reserve(v_world_size_sq);
	for (std::size_t y = 0; y < v_world_size; y++)
	{
		for (std::size_t x = 0; x < v_world_size; x++)
		{
			const float& height = v_vert_array[x + y * v_world_size];
			const float vert_x = -(static_cast<float>(x) * 2.0f) + vf_world_size;
			const float vert_y = -(static_cast<float>(y) * 2.0f) + vf_world_size;

			tMesh->vertices.push_back(glm::vec3(vert_x, vert_y, height));

			if (SharedConverterSettings::ExportNormals)
			{
				tMesh->normals.push_back(glm::vec3(0.0f));
				normal_div.push_back(1);
			}
		}
	}

	if (SharedConverterSettings::ExportUvs)
	{
		const float v_uv_size = static_cast<float>(v_world_size - 1);

		tMesh->uvs.reserve(v_world_size_sq);
		for (std::size_t y = 0; y < v_world_size; y++)
		{
			for (std::size_t x = 0; x < v_world_size; x++)
			{
				const float u = static_cast<float>(x) / v_uv_size;
				const float v = static_cast<float>(y) / v_uv_size;

				tMesh->uvs.push_back(glm::vec2(u, v));
			}
		}
	}

	SubMeshData* pSubMesh = new SubMeshData(0);

	pSubMesh->has_normals = SharedConverterSettings::ExportNormals;
	pSubMesh->has_uvs = SharedConverterSettings::ExportUvs;

	//generate normals
	pSubMesh->m_DataIdx.reserve(v_world_size_sq);
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
				const glm::vec3& p1 = tMesh->vertices[h00];
				const glm::vec3& p2 = tMesh->vertices[h01];
				const glm::vec3& p3 = tMesh->vertices[h10];
				const glm::vec3& p4 = tMesh->vertices[h11];

				const glm::vec3 t1_norm = CalculateNormalVector(p1, p3, p2); //first_triangle
				const glm::vec3 t2_norm = CalculateNormalVector(p2, p3, p4); //second_triangle

				tMesh->normals[h00] += t1_norm;
				tMesh->normals[h01] += (t1_norm + t2_norm);
				tMesh->normals[h10] += (t1_norm + t2_norm);
				tMesh->normals[h11] += t2_norm;

				normal_div[h00] += 1;
				normal_div[h01] += 2;
				normal_div[h10] += 2;
				normal_div[h11] += 1;
			}

			const VertexData vert1 = { h00, SharedConverterSettings::ExportUvs ? h00 : 0, SharedConverterSettings::ExportNormals ? h00 : 0 };
			const VertexData vert2 = { h01, SharedConverterSettings::ExportUvs ? h01 : 0, SharedConverterSettings::ExportNormals ? h01 : 0 };
			const VertexData vert3 = { h10, SharedConverterSettings::ExportUvs ? h10 : 0, SharedConverterSettings::ExportNormals ? h10 : 0 };
			const VertexData vert4 = { h11, SharedConverterSettings::ExportUvs ? h11 : 0, SharedConverterSettings::ExportNormals ? h11 : 0 };

			pSubMesh->m_DataIdx.push_back({ vert1, vert3, vert2 });
			pSubMesh->m_DataIdx.push_back({ vert2, vert3, vert4 });
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

				tMesh->normals[h00] = glm::normalize(tMesh->normals[h00] / static_cast<float>(normal_div[h00]));
				tMesh->normals[h01] = glm::normalize(tMesh->normals[h01] / static_cast<float>(normal_div[h01]));
				tMesh->normals[h10] = glm::normalize(tMesh->normals[h10] / static_cast<float>(normal_div[h10]));
				tMesh->normals[h11] = glm::normalize(tMesh->normals[h11] / static_cast<float>(normal_div[h11]));
			}
		}
	}

	tMesh->subMeshData.push_back(pSubMesh);

	return tMesh;
}

void SMWorld::WriteTerrain(std::ofstream& model, WriterOffsetData& v_offset, const std::vector<float>& height_map) const
{
	DebugOutL("Writing terrain...");
	ProgCounter::SetState(ProgState::WritingGroundMesh, 0);

	//Used to generate the mamterial for ground terrain mesh
	SMGroundTerrainData* v_tmp_terrain_data = new SMGroundTerrainData();

	Model* v_terrain = this->GenerateTerrainMesh(height_map);

	const glm::mat4 v_rotation = glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
	v_terrain->WriteToFile(v_rotation, v_offset, model, v_tmp_terrain_data);

	delete v_terrain;
	delete v_tmp_terrain_data;
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

			glm::vec3 v_clutter_pos;
			v_clutter_pos.x = -((float)(v_world_sz - x - 1) * 0.5f) + v_grid_sz_f + x_offset;
			v_clutter_pos.y = -((float)(v_world_sz - y - 1) * 0.5f) + v_grid_sz_f + y_offset;
			v_clutter_pos.z = Math::GetHeightPointBox(height_map, v_world_sz, v_grid_sz, v_x_clamp, v_y_clamp);

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
	const int v_half_width = static_cast<int>(m_width / 2);
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

	std::unordered_map<std::string, ObjectTexData> v_tex_data = {};

	for (const WorldCellData& v_cell : m_cellMap)
	{
		const TilePart* v_part = v_cell.part;
		if (!v_part) continue;

		v_part->FillTextureMap(v_tex_data);
		ProgCounter::ProgressMax = v_tex_data.size();
	}

	{
		ObjectTexData v_tileGroundTextureData;
		v_tileGroundTextureData.m_tex_color = 0xffffff;

		if (TileConverterSettings::ExportGroundTextures)
		{
			v_tileGroundTextureData.m_textures.dif = L"./GroundTexture_Dif.jpg";
			v_tileGroundTextureData.m_textures.asg = L"./GroundTexture_Asg.jpg";
			v_tileGroundTextureData.m_textures.nor = L"./GroundTexture_Nor.jpg";
		}

		v_tex_data["TileGroundTerrain"] = v_tileGroundTextureData;
	}

	MtlFileWriter::Write(path, v_tex_data);
}

bool SMWorld::WriteObjFile(const std::wstring& dir_path, const std::wstring& file_name, const std::wstring& mtl_name) const
{
	const std::wstring v_output_path = dir_path + file_name + L".obj";

	std::ofstream v_output_model(v_output_path);
	if (!v_output_model.is_open())
	{
		DebugOutL("Fail!");
		return false;
	}

	if (SharedConverterSettings::ExportMaterials)
	{
		const std::string v_mtl_header = "mtllib " + String::ToUtf8(mtl_name) + "\n";
		v_output_model.write(v_mtl_header.c_str(), v_mtl_header.size());
	}

	WriterOffsetData v_offset_data;

	{
		std::vector<float> v_height_array;
		this->GetVertexHeight(v_height_array);

		this->WriteTerrain(v_output_model, v_offset_data, v_height_array);
		this->WriteClutter(v_output_model, v_offset_data, v_height_array);

		ProgCounter::SetState(ProgState::MemoryCleanup, 0);
	}

	this->WriteAssets(v_output_model, v_offset_data);
	v_output_model.close();

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
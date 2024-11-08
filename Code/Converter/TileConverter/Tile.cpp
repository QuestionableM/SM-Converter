#include "Tile.hpp"
#include "TileHeader.hpp"

#include "ObjectDatabase/GroundTextureDatabase.hpp"
#include "ObjectDatabase/ProgCounter.hpp"
#include "ObjectDatabase/ObjectData.hpp"

#include "Converter/TileConverter/TileConverter.hpp"
#include "Converter/Entity/GroundTerrainData.hpp"
#include "Converter/MtlFileWriter.hpp"

#include "UStd/UnmanagedFilesystem.hpp"

#include "Utils/GlmUnmanaged.hpp"
#include "Utils/clr_include.hpp"
#include "Utils/WinInclude.hpp"
#include "Utils/Console.hpp"
#include "Utils/String.hpp"

SM_UNMANAGED_CODE

#include <PerlinNoise\PerlinNoise.hpp>
#include <FreeImage.h>

Tile::Tile() :
	m_version(0),
	m_hasTerrain(false),
	m_creatorId(0),
	m_width(0),
	m_height(0),
	m_tiles()
{}

Tile::Tile(const TileHeader& header) :
	m_version(header.m_data.version),
	m_hasTerrain(((header.m_data.type >> 0x18) & 1) == 0),
	m_creatorId(header.m_data.creator_id),
	m_width(header.m_data.width),
	m_height(header.m_data.height),
	m_tiles(std::size_t(m_width * m_height))
{
	const std::size_t v_tileCount = m_tiles.size();
	for (std::size_t a = 0; a < m_tiles.size(); a++)
		m_tiles[a] = new TilePart();
}

Tile::Tile(Tile&& other) noexcept :
	m_version(other.m_version),
	m_hasTerrain(other.m_hasTerrain),
	m_creatorId(other.m_creatorId),
	m_width(other.m_width),
	m_height(other.m_height),
	m_tiles(std::move(other.m_tiles))
{}

Tile::~Tile()
{
	for (TilePart* v_pCurTile : m_tiles)
		delete v_pCurTile;
}

void Tile::operator=(Tile&& other) noexcept
{
	m_version = other.m_version;
	m_hasTerrain = other.m_hasTerrain;
	m_creatorId = other.m_creatorId;
	m_width = other.m_width;
	m_height = other.m_height;
	m_tiles = std::move(other.m_tiles);
}

void Tile::SetPart(int x, int y, TilePart* part)
{
	assert(part == nullptr);
	m_tiles[x + y * m_width] = part;
}

TilePart* Tile::GetPart(int x, int y) const
{
	return m_tiles[x + y * m_width];
}

TilePart* Tile::GetPartSafe(int x, int y) const noexcept
{
	if (x >= m_width || y >= m_height)
		return nullptr;

	return m_tiles[x + y * m_width];
}

void Tile::Resize(int width, int height)
{
	assert(width < 1 || height < 1);

	std::vector<TilePart*> nTileArray(static_cast<std::size_t>(width * height));

	const int min_height = std::min(height, m_height);
	const int min_width = std::min(width, m_width);

	for (int y = 0; y < min_height; y++)
	{
		for (int x = 0; x < min_width; x++)
		{
			nTileArray[x + y * width] = this->GetPart(x, y);
		}
	}

	for (std::size_t a = 0; a < nTileArray.size(); a++)
	{
		if (nTileArray[a] == nullptr)
			nTileArray[a] = new TilePart();
	}

	this->m_width = width;
	this->m_height = height;
	this->m_tiles = nTileArray;
}

std::vector<float> Tile::GetVertexHeight() const
{
	const int w = m_width * 32 + 1;
	const int h = m_height * 32 + 1;

	std::vector<float> float_array(static_cast<std::size_t>(w * h));

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			TilePart* part = this->GetPart(x, y);

			const int idx = x * 32 + y * 32 * h;
			for (int a = 0; a < 33; a++)
			{
				std::memcpy(float_array.data() + (idx + a * w), part->m_vertexHeight.data() + (a * 33), 33 * sizeof(float));
			}
		}
	}

	return float_array;
}

std::vector<int> Tile::GetVertexColor() const
{
	const int w = m_width * 32 + 1;
	const int h = m_height * 32 + 1;

	std::vector<int> vert_colors(static_cast<std::size_t>(w * h));

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			TilePart* part = this->GetPart(x, y);

			const int idx = x * 32 + y * 32 * h;
			for (int a = 0; a < 33; a++)
			{
				std::memcpy(vert_colors.data() + (idx + a * w), part->m_vertexColor.data() + (a * 33), 33 * sizeof(int));
			}
		}
	}

	return vert_colors;
}

std::vector<SMTileClutter*> Tile::GetClutter() const
{
	const int w = m_width * 128;
	const int h = m_height * 128;

	std::vector<SMTileClutter*> clutter_bytes(static_cast<std::size_t>(w * h));

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			TilePart* part = this->GetPart(x, y);

			const int idx = x * 128 + y * 128 * h;
			for (int a = 0; a < 128; a++)
			{
				std::memcpy(clutter_bytes.data() + (idx + a * w), part->m_clutterMap.data() + (a * 128), 128 * sizeof(std::size_t));
			}
		}
	}

	return clutter_bytes;
}

std::vector<long long> Tile::GetGround() const
{
	const int w = m_width * 64 + 1;
	const int h = m_height * 64 + 1;

	std::vector<long long> ground_bytes(static_cast<std::size_t>(w * h));

	for (int y = 0; y < m_height; y++)
	{
		for (int x = 0; x < m_width; x++)
		{
			TilePart* part = this->GetPart(x, y);

			const int idx = x * 64 + y * 64 * h;
			for (int a = 0; a < 64; a++)
			{
				std::memcpy(ground_bytes.data() + (idx + a * w), part->m_ground.data() + (a * 65), 65 * sizeof(long long));
			}
		}
	}

	return ground_bytes;
}

inline float lerp(float s, float e, float t)
{
	return s + (e - s) * t;
}

inline float blerp(float c00, float c10, float c01, float c11, float tx, float ty)
{
	return lerp(lerp(c00, c10, tx), lerp(c01, c11, tx), ty);
}

inline float GetHeightPoint(
	const std::vector<float>& vec,
	int vec_width,
	int vec_height,
	int gridSzX,
	int gridSzY,
	float x,
	float y)
{
	const float gx = x / float(vec_height) * gridSzX;
	const float gy = y / float(vec_width) * gridSzY;

	const int gxi = int(gx);
	const int gyi = int(gy);

	const float c00 = vec[(gxi    ) + (gyi    ) * (gridSzX + 1)];
	const float c10 = vec[(gxi + 1) + (gyi    ) * (gridSzX + 1)];
	const float c01 = vec[(gxi    ) + (gyi + 1) * (gridSzX + 1)];
	const float c11 = vec[(gxi + 1) + (gyi + 1) * (gridSzX + 1)];

	return blerp(c00, c10, c01, c11, gx - gxi, gy - gyi);
}

inline glm::vec3 CalculateNormalVector(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
	//p2 is the new origin (can be changed later)
	const glm::vec3 a = p2 - p1;
	const glm::vec3 b = p3 - p1;

	return glm::normalize(glm::cross(a, b));
}

void Tile::GenerateTerrainMesh(Model& outModel, const std::vector<float>& height_map) const
{
	DebugOutL("Generating terrain mesh...");

	const std::size_t tWidth  = static_cast<std::size_t>(m_width)  * 32 + 1;
	const std::size_t tHeight = static_cast<std::size_t>(m_height) * 32 + 1;

	const float hWidth  = static_cast<float>(m_width) * 32.0f;
	const float hHeight = static_cast<float>(m_height) * 32.0f;

	const bool l_ExportNormals = SharedConverterSettings::ExportNormals;
	const bool l_ExportUvs     = SharedConverterSettings::ExportUvs;

	std::vector<std::size_t> normal_div;
	if (l_ExportNormals)
	{
		outModel.m_normals.reserve(tWidth * tHeight);
		normal_div.reserve(tWidth * tHeight);
	}

	outModel.m_vertices.reserve(tWidth * tHeight);
	for (std::size_t y = 0; y < tHeight; y++)
	{
		for (std::size_t x = 0; x < tWidth; x++)
		{
			const float height = height_map[x + y * tWidth];
			const float vert_x = -(static_cast<float>(x) * 2.0f) + hWidth;
			const float vert_y = -(static_cast<float>(y) * 2.0f) + hHeight;

			outModel.m_vertices.emplace_back(vert_x, vert_y, height);

			if (l_ExportNormals)
			{
				outModel.m_normals.emplace_back(0.0f);
				normal_div.push_back(1);
			}
		}
	}

	if (l_ExportUvs)
	{
		const float uvWidth = static_cast<float>(tWidth - 1);
		const float uvHeight = static_cast<float>(tHeight - 1);

		outModel.m_uvs.reserve(tWidth * tHeight);
		for (std::size_t y = 0; y < tHeight; y++)
		{
			for (std::size_t x = 0; x < tWidth; x++)
			{
				const float u = static_cast<float>(x) / uvWidth;
				const float v = static_cast<float>(y) / uvHeight;

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
	v_newSubMesh.m_dataIdx.reserve(tWidth * tHeight);
	for (std::size_t y = 0; y < tHeight - 1; y++)
	{
		for (std::size_t x = 0; x < tWidth - 1; x++)
		{
			const std::size_t h00 = (x    ) + (y    ) * tWidth;
			const std::size_t h01 = (x    ) + (y + 1) * tWidth;
			const std::size_t h10 = (x + 1) + (y    ) * tWidth;
			const std::size_t h11 = (x + 1) + (y + 1) * tWidth;

			if (l_ExportNormals)
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

			const VertexData vert1 = { h00, l_ExportUvs ? h00 : 0, l_ExportNormals ? h00 : 0 };
			const VertexData vert2 = { h01, l_ExportUvs ? h01 : 0, l_ExportNormals ? h01 : 0 };
			const VertexData vert3 = { h10, l_ExportUvs ? h10 : 0, l_ExportNormals ? h10 : 0 };
			const VertexData vert4 = { h11, l_ExportUvs ? h11 : 0, l_ExportNormals ? h11 : 0 };

			v_newSubMesh.m_dataIdx.emplace_back(std::initializer_list{ vert1, vert3, vert2 });
			v_newSubMesh.m_dataIdx.emplace_back(std::initializer_list{ vert2, vert3, vert4 });
		}
	}

	if (l_ExportNormals)
	{
		//make normals smooth
		for (std::size_t y = 0; y < tHeight - 1; y++)
		{
			for (std::size_t x = 0; x < tWidth - 1; x++)
			{
				const std::size_t h00 = (x    ) + (y    ) * tWidth;
				const std::size_t h01 = (x    ) + (y + 1) * tWidth;
				const std::size_t h10 = (x + 1) + (y    ) * tWidth;
				const std::size_t h11 = (x + 1) + (y + 1) * tWidth;

				outModel.m_normals[h00] = glm::normalize(outModel.m_normals[h00] / static_cast<float>(normal_div[h00]));
				outModel.m_normals[h01] = glm::normalize(outModel.m_normals[h01] / static_cast<float>(normal_div[h01]));
				outModel.m_normals[h10] = glm::normalize(outModel.m_normals[h10] / static_cast<float>(normal_div[h10]));
				outModel.m_normals[h11] = glm::normalize(outModel.m_normals[h11] / static_cast<float>(normal_div[h11]));
			}
		}
	}
}

void Tile::WriteTerrain(std::ofstream& model, WriterOffsetData& mOffset, const std::vector<float>& height_map) const
{
	if (!m_hasTerrain) return;

	DebugOutL("Writing terrain...");
	ProgCounter::SetState(ProgState::WritingGroundMesh, 0);

	// Used to generate the material for ground terrain mesh
	SMGroundTerrainData v_tempTerrainData;
	Model v_terrainSurface;

	this->GenerateTerrainMesh(v_terrainSurface, height_map);
	v_terrainSurface.WriteToFile(glm::mat4(1.0f), mOffset, model, &v_tempTerrainData);
}

void Tile::WriteClutter(std::ofstream& model, WriterOffsetData& mOffset, const std::vector<float>& height_map) const
{
	if (!m_hasTerrain || !TileConverterSettings::ExportClutter) return;

	DebugOutL("Writing clutter...");

	std::vector<SMTileClutter*> tile_clutter = this->GetClutter();

	const int clWidth  = m_width  * 128;
	const int clHeight = m_height * 128;

	const float clWidthF  = static_cast<float>(clWidth);
	const float clHeightF = static_cast<float>(clHeight);

	const int gridSizeX = m_width  * 32;
	const int gridSizeY = m_height * 32;

	const float tWidth  = static_cast<float>(gridSizeX);
	const float tHeight = static_cast<float>(gridSizeY);

	//initialize perlin noise
	const siv::PerlinNoise rotation_noise(1337u);
	const siv::PerlinNoise scale_noise(1488u);
	const siv::PerlinNoise x_noise(6842u);
	const siv::PerlinNoise y_noise(1813u);

	ProgCounter::SetState(ProgState::WritingClutter, static_cast<std::size_t>(clWidth * clHeight));
	for (int y = 0; y < clWidth; y++)
	{
		for (int x = 0; x < clHeight; x++)
		{
			SMTileClutter* tClutter = tile_clutter[x + y * clWidth];
			if (!tClutter) continue;

			const float x_offset = (float)x_noise.octave2D_11((double)x * 91.42f, (double)y * 83.24f, 4) * 0.5f;
			const float y_offset = (float)y_noise.octave2D_11((double)x * 73.91f, (double)y * 98.46f, 4) * 0.5f;

			const float xPosClamp = std::clamp<float>((float)x + x_offset, 0.0f, (float)clWidth);
			const float yPosClamp = std::clamp<float>((float)y + y_offset, 0.0f, (float)clHeight);

			const glm::vec3 tClutterPos(
				-((float)x * 0.5f) + tWidth + x_offset,
				-((float)y * 0.5f) + tHeight + y_offset,
				GetHeightPoint(height_map, clWidth, clHeight, gridSizeX, gridSizeY, xPosClamp, yPosClamp)
			);

			const float rot_angle = (float)rotation_noise.octave2D_11((double)x * 15.0, (double)y * 17.14, 4) * glm::two_pi<float>();
			const float rand_scale = (float)scale_noise.octave2D_11((double)x * 54.4f, (double)y * 24.54, 8) * tClutter->ScaleVariance();

			tClutter->SetPosition(tClutterPos);
			tClutter->SetRotation(glm::rotate(glm::quat(), rot_angle, glm::vec3(0.0f, 0.0f, 1.0f)));
			tClutter->SetSize(glm::vec3(0.25f - (rand_scale * 0.25f)));
			tClutter->WriteObjectToFile(model, mOffset, glm::mat4(1.0f));

			ProgCounter::ProgressValue++;
		}
	}
}

std::size_t Tile::GetAmountOfObjects() const
{
	std::size_t output = 0;

	for (const TilePart* tPart : m_tiles)
		output += tPart->GetAmountOfObjects();

	return output;
}

void Tile::WriteAssets(std::ofstream& model, WriterOffsetData& mOffset) const
{
	DebugOutL("Writing assets...");

	ProgCounter::SetState(ProgState::WritingObjects, this->GetAmountOfObjects());
	for (int y = 0; y < m_width; y++)
	{
		for (int x = 0; x < m_height; x++)
		{
			this->GetPart(x, y)->WriteToFile(model, mOffset, m_width, m_height, x, y);
		}
	}
}

void Tile::WriteMaterials(const std::wstring& dir) const
{
	if (!m_hasTerrain || !SharedConverterSettings::ExportMaterials) return;

	DebugOutL("Writing materials...");
	ProgCounter::SetState(ProgState::WritingMaterialMaps, 0);

	const std::vector<long long> ground_data = this->GetGround();

	const int gnd_width  = m_width  * 64 + 1;
	const int gnd_height = m_height * 64 + 1;

	for (std::size_t mat_id = 0; mat_id < 2; mat_id++)
	{
		FIBITMAP* v_materialData = FreeImage_Allocate(gnd_width, gnd_height, 32);
		if (!v_materialData)
		{
			DebugErrorL("Couldn't allocate memory for material id: ", mat_id);
			continue;
		}

		const std::size_t v_curOffset = 32 * mat_id;
		RGBQUAD v_pixelData;

		for (int y = 0; y < gnd_width; y++)
		{
			for (int x = 0; x < gnd_height; x++)
			{
				const long long cur_data = ground_data[x + y * gnd_width];
				const long cur_chunk = static_cast<long>(cur_data >> v_curOffset);

				v_pixelData.rgbRed      = static_cast<Byte>(cur_chunk >> 0);
				v_pixelData.rgbGreen    = static_cast<Byte>(cur_chunk >> 8);
				v_pixelData.rgbBlue     = static_cast<Byte>(cur_chunk >> 16);
				v_pixelData.rgbReserved = static_cast<Byte>(cur_chunk >> 24);

				FreeImage_SetPixelColor(v_materialData, x, y, &v_pixelData);
			}
		}

		const std::wstring v_matOutputName = dir + L"MaterialMap" + std::to_wstring(mat_id) + L".tga";
		FreeImage_SaveU(FREE_IMAGE_FORMAT::FIF_TARGA, v_materialData, v_matOutputName.c_str());
		FreeImage_Unload(v_materialData);
	}
}

void Tile::WriteColorMap(const std::wstring& dir) const
{
	if (!m_hasTerrain || !SharedConverterSettings::ExportMaterials) return;

	DebugOutL("Writing color map...");
	ProgCounter::SetState(ProgState::WritingColorMap, 0);

	const std::vector<int> vert_colors = this->GetVertexColor();

	const int col_width  = m_width  * 32 + 1;
	const int col_height = m_height * 32 + 1;

	FIBITMAP* v_colorMapData = FreeImage_Allocate(col_width, col_height, 24);
	if (!v_colorMapData)
	{
		DebugErrorL("Couldn't allocate memory for color map!");
		return;
	}

	RGBQUAD v_pixelColor;

	for (int y = 0; y < col_height; y++)
	{
		for (int x = 0; x < col_width; x++)
		{
			const int cur_data = vert_colors[x + y * col_width];

			v_pixelColor.rgbRed   = static_cast<Byte>(cur_data);
			v_pixelColor.rgbGreen = static_cast<Byte>(cur_data >> 8);
			v_pixelColor.rgbBlue  = static_cast<Byte>(cur_data >> 16);

			FreeImage_SetPixelColor(v_colorMapData, x, y, &v_pixelColor);
		}
	}

	const std::wstring v_colorMapPath = dir + L"ColorMap.tga";
	FreeImage_SaveU(FREE_IMAGE_FORMAT::FIF_TARGA, v_colorMapData, v_colorMapPath.c_str());
	FreeImage_Unload(v_colorMapData);
}

void Tile::SampleTextures(
	GroundTexture* tex1,
	GroundTexture* out_tex,
	const std::vector<float>& material_map,
	int gnd_width,
	int gnd_height) const
{
	constexpr const float mul_div = 1.0f / 255.0f;

	const int tex1_x = tex1->GetWidth();
	const int tex1_y = tex1->GetHeight();

	const int out_tex_x = out_tex->GetWidth();
	const int out_tex_y = out_tex->GetHeight();

	RGBQUAD m_pixelData;

	for (int y = 0; y < out_tex_y; y++)
	{
		const float pY = static_cast<float>(y);

		const int p_tex1_y = y % tex1_y;
		const int p_tex2_y = y % out_tex_y;

		for (int x = 0; x < out_tex_x; x++)
		{
			const float pX = static_cast<float>(x);

			const float sample_data = GetHeightPoint(material_map, out_tex_x, out_tex_y, gnd_width, gnd_height, pX, pY);
			if (sample_data == 0.0f) continue;

			const int p_tex1_x = x % tex1_x;
			const int p_tex2_x = x % out_tex_x;

			tex1->GetByte(p_tex1_x, p_tex1_y, &m_pixelData);

			float r_sample = static_cast<float>(m_pixelData.rgbRed)   * mul_div;
			float g_sample = static_cast<float>(m_pixelData.rgbGreen) * mul_div;
			float b_sample = static_cast<float>(m_pixelData.rgbBlue)  * mul_div;

			if (sample_data != 1.0f)
			{
				out_tex->GetByte(p_tex2_x, p_tex2_y, &m_pixelData);

				const float def_r = static_cast<float>(m_pixelData.rgbRed)   * mul_div;
				const float def_g = static_cast<float>(m_pixelData.rgbGreen) * mul_div;
				const float def_b = static_cast<float>(m_pixelData.rgbBlue)  * mul_div;

				r_sample = lerp(def_r, r_sample, sample_data);
				g_sample = lerp(def_g, g_sample, sample_data);
				b_sample = lerp(def_b, b_sample, sample_data);
			}

			m_pixelData.rgbRed   = static_cast<Byte>(r_sample * 255.0f);
			m_pixelData.rgbGreen = static_cast<Byte>(g_sample * 255.0f);
			m_pixelData.rgbBlue  = static_cast<Byte>(b_sample * 255.0f);

			out_tex->SetByte(x, y, &m_pixelData);
		}
	}
}

void Tile::FillGndTexture(GroundTexture* mGndTex, const std::size_t& tex_id) const
{
	GroundTexture* pDefTex = GroundTextureDatabase::GetDefaultTexture(tex_id);
	if (!pDefTex->LoadImageData())
		return;

	int v_widthDiv = std::max(m_width / 2, 1);
	int v_heightDiv = std::max(m_height / 2, 1);
	if (!TileConverterSettings::Export8kGroundTextures)
	{
		v_widthDiv = m_width * 2;
		v_heightDiv = m_height * 2;
	}

	pDefTex->Resize(pDefTex->GetWidth() / v_widthDiv, pDefTex->GetHeight() / v_heightDiv);

	const int gnd_tex_x = mGndTex->GetWidth();
	const int gnd_tex_y = mGndTex->GetHeight();

	const int def_size_x = pDefTex->GetWidth();
	const int def_size_y = pDefTex->GetHeight();

	const int def_x_amount = gnd_tex_x / def_size_x;
	const int def_y_amount = gnd_tex_y / def_size_y;

	for (int y = 0; y < def_y_amount; y++)
	{
		const int v_img_y_offset = y * def_size_y;

		for (int x = 0; x < def_x_amount; x++)
		{
			FreeImage_Paste(mGndTex->Data(), pDefTex->Data(), x * def_size_x, v_img_y_offset, 256);
		}
	}

	pDefTex->Clear();
}

struct MaterialData
{
	std::vector<float> MatData;
	bool HasMatData;
};

void Tile::FillMaterialMap(std::array<MaterialData, 8>& mat_data) const
{
	const std::vector<long long> ground_data = this->GetGround();

	const std::size_t gnd_width  = static_cast<std::size_t>(m_width ) * 64 + 1;
	const std::size_t gnd_height = static_cast<std::size_t>(m_height) * 64 + 1;

	for (std::size_t mat_id = 0; mat_id < 8; mat_id++)
	{
		const std::size_t mat_offset = 8 * mat_id;

		MaterialData& v_curMaterial = mat_data[mat_id];

		std::vector<float>& material_vec = v_curMaterial.MatData;
		material_vec.resize(gnd_width * gnd_height);

		for (std::size_t y = 0; y < gnd_height; y++)
		{
			for (std::size_t x = 0; x < gnd_width; x++)
			{
				const long long cur_data = ground_data[x + y * gnd_width];
				const Byte cur_chunk = static_cast<Byte>(cur_data >> mat_offset);

				material_vec[x + y * gnd_width] = static_cast<float>(cur_chunk) / 255.0f;

				v_curMaterial.HasMatData |= (cur_chunk != 0);
			}
		}
	}
}

static const std::wstring GroundTextureNames[] = { L"Dif", L"Asg", L"Nor" };
void Tile::WriteGroundTextures(const std::wstring& dir) const
{
	if (!m_hasTerrain || !TileConverterSettings::ExportGroundTextures) return;

	std::array<MaterialData, 8> v_materialMap = {};
	this->FillMaterialMap(v_materialMap);

	const int gnd_width2  = m_width  * 64;
	const int gnd_height2 = m_height * 64;

	const int v_gndTexResolution = TileConverterSettings::Export8kGroundTextures ? 8192 : 4096;

	int v_widthDiv = std::max(m_width / 2, 1);
	int v_heightDiv = std::max(m_height / 2, 1);
	if (!TileConverterSettings::Export8kGroundTextures)
	{
		v_widthDiv = m_width * 2;
		v_heightDiv = m_height * 2;
	}

	for (std::size_t texture_id = 0; texture_id < 3; texture_id++)
	{
		const std::size_t writing_gnd_idx = static_cast<std::size_t>(ProgState::FillingGndDif) + (3 * texture_id);

		GroundTexture gnd_tex;
		if (!gnd_tex.AllocateMemory(v_gndTexResolution, v_gndTexResolution))
		{
			DebugErrorL("Couldn't allocate a ", v_gndTexResolution, ", ", v_gndTexResolution, " texture!");
			continue;
		}

		ProgCounter::SetState(static_cast<ProgState>(writing_gnd_idx), 8);
		this->FillGndTexture(&gnd_tex, texture_id);

		ProgCounter::SetState(static_cast<ProgState>(writing_gnd_idx + 1), 8);
		for (std::size_t mat_id = 0; mat_id < 8; mat_id++)
		{
			const MaterialData& v_curMat = v_materialMap[mat_id];
			if (v_curMat.HasMatData)
			{
				GroundTexture* v_gndTexPtr = GroundTextureDatabase::GetTexture(mat_id, texture_id);
				if (!v_gndTexPtr->LoadImageData())
					continue;

				v_gndTexPtr->Resize(v_gndTexPtr->GetWidth() / v_widthDiv, v_gndTexPtr->GetHeight() / v_heightDiv);

				this->SampleTextures(v_gndTexPtr, &gnd_tex, v_curMat.MatData, gnd_width2, gnd_height2);

				v_gndTexPtr->Clear();
			}

			ProgCounter::ProgressValue++;
		}

		ProgCounter::SetState(static_cast<ProgState>(writing_gnd_idx + 2), 0);
		gnd_tex.WriteToFile(dir + L"GroundTexture_" + GroundTextureNames[texture_id] + L".jpg");
	}
}

void Tile::WriteModel(const std::wstring& file_path, const std::wstring& mtl_name) const
{
	std::ofstream v_outputModel(file_path);
	if (!v_outputModel.is_open()) return;

	if (SharedConverterSettings::ExportMaterials)
	{
		const std::string v_mtlHeader = "mtllib " + String::ToUtf8(mtl_name) + "\n";
		v_outputModel.write(v_mtlHeader.c_str(), v_mtlHeader.size());
	}

	WriterOffsetData v_offsetData;

	{
		const std::vector<float> v_heightArray = this->GetVertexHeight();

		this->WriteTerrain(v_outputModel, v_offsetData, v_heightArray);
		this->WriteClutter(v_outputModel, v_offsetData, v_heightArray);
	}

	this->WriteAssets(v_outputModel, v_offsetData);
}

void Tile::WriteToFile(const std::wstring& dir_path, const std::wstring& file_name) const
{
	const std::wstring v_mtlName = file_name + L".mtl";

	this->WriteModel(dir_path + file_name + L".obj", v_mtlName);
	this->WriteMtlFile(dir_path + v_mtlName);

	this->WriteMaterials(dir_path);
	this->WriteColorMap(dir_path);
	this->WriteGroundTextures(dir_path);

	DebugOutL("Finished!");
}

void Tile::WriteMtlFile(const std::wstring& path) const
{
	if (!SharedConverterSettings::ExportMaterials) return;

	DebugOutL("Writing an mtl file...");
	ProgCounter::SetState(ProgState::WritingMtlFile, 0);

	std::unordered_map<std::string, ObjectTexData> v_texData = {};

	for (const TilePart* v_tPart : m_tiles)
	{
		v_tPart->FillTextureMap(v_texData);
		ProgCounter::ProgressMax = v_texData.size();
	}

	{
		ObjectTexData v_tileGroundTextureData(0xffffff);

		if (TileConverterSettings::ExportGroundTextures)
		{
			v_tileGroundTextureData.m_textures.m_dif = L"./GroundTexture_Dif.jpg";
			v_tileGroundTextureData.m_textures.m_asg = L"./GroundTexture_Asg.jpg";
			v_tileGroundTextureData.m_textures.m_nor = L"./GroundTexture_Nor.jpg";
		}

		v_texData["TileGroundTerrain"] = std::move(v_tileGroundTextureData);
	}

	MtlFileWriter::Write(path, v_texData);
}
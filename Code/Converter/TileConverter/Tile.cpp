#include "Tile.hpp"

#include "ObjectDatabase\GroundTextureDatabase.hpp"
#include "ObjectDatabase\ProgCounter.hpp"
#include "ObjectDatabase\ObjectData.hpp"

#include "Converter\TileConverter\TileConverter.hpp"
#include "Converter\Entity\GroundTerrainData.hpp"
#include "Converter\MtlFileWriter.hpp"

#include "Utils\WinInclude.hpp"
#include "Utils\Console.hpp"
#include "Utils\String.hpp"

#include <PerlinNoise\PerlinNoise.hpp>
#include <gtc\matrix_transform.hpp>
#include <FreeImage.h>

#include <filesystem>
namespace fs = std::filesystem;

Tile::Tile(const int& width, const int& height)
{
	this->m_Width = width;
	this->m_Height = height;
	this->m_Tiles.resize(static_cast<std::size_t>(m_Width * m_Height));

	for (std::size_t a = 0; a < m_Tiles.size(); a++)
		m_Tiles[a] = new TilePart(this);
}

Tile::~Tile()
{
	for (std::size_t a = 0; a < m_Tiles.size(); a++)
		delete m_Tiles[a];
}


void Tile::Resize(const int& width, const int& height)
{
	assert(width < 1 || height < 1);

	std::vector<TilePart*> nTileArray = {};
	nTileArray.resize(static_cast<std::size_t>(width * height));

	const int min_height = std::min(height, m_Height);
	const int min_width = std::min(width, m_Width);

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
			nTileArray[a] = new TilePart(this);
	}

	this->m_Width = width;
	this->m_Height = height;
	this->m_Tiles = nTileArray;
}

std::vector<float> Tile::GetVertexHeight() const
{
	const int w = m_Width * 32 + 1;
	const int h = m_Height * 32 + 1;

	std::vector<float> float_array = {};
	float_array.resize(static_cast<std::size_t>(w * h));

	for (int y = 0; y < m_Height; y++)
	{
		for (int x = 0; x < m_Width; x++)
		{
			TilePart* part = this->GetPart(x, y);

			const int idx = x * 32 + y * 32 * h;
			for (int a = 0; a < 33; a++)
			{
				std::memcpy(float_array.data() + (idx + a * w), part->m_VertexHeight.data() + (a * 33), 33 * sizeof(float));
			}
		}
	}

	return float_array;
}

std::vector<int> Tile::GetVertexColor() const
{
	const int w = m_Width * 32 + 1;
	const int h = m_Height * 32 + 1;

	std::vector<int> vert_colors = {};
	vert_colors.resize(static_cast<std::size_t>(w * h));

	for (int y = 0; y < m_Height; y++)
	{
		for (int x = 0; x < m_Width; x++)
		{
			TilePart* part = this->GetPart(x, y);

			const int idx = x * 32 + y * 32 * h;
			for (int a = 0; a < 33; a++)
			{
				std::memcpy(vert_colors.data() + (idx + a * w), part->m_VertexColor.data() + (a * 33), 33 * sizeof(int));
			}
		}
	}

	return vert_colors;
}

std::vector<SMTileClutter*> Tile::GetClutter() const
{
	const int w = m_Width * 128;
	const int h = m_Height * 128;

	std::vector<SMTileClutter*> clutter_bytes = {};
	clutter_bytes.resize(static_cast<std::size_t>(w * h));

	for (int y = 0; y < m_Height; y++)
	{
		for (int x = 0; x < m_Width; x++)
		{
			TilePart* part = this->GetPart(x, y);

			const int idx = x * 128 + y * 128 * h;
			for (int a = 0; a < 128; a++)
			{
				std::memcpy(clutter_bytes.data() + (idx + a * w), part->m_ClutterMap.data() + (a * 128), 128 * sizeof(std::size_t));
			}
		}
	}

	return clutter_bytes;
}

std::vector<long long> Tile::GetGround() const
{
	const int w = m_Width * 64 + 1;
	const int h = m_Height * 64 + 1;

	std::vector<long long> ground_bytes = {};
	ground_bytes.resize(static_cast<std::size_t>(w * h));

	for (int y = 0; y < m_Height; y++)
	{
		for (int x = 0; x < m_Width; x++)
		{
			TilePart* part = this->GetPart(x, y);

			const int idx = x * 64 + y * 64 * h;
			for (int a = 0; a < 64; a++)
			{
				std::memcpy(ground_bytes.data() + (idx + a * w), part->m_Ground.data() + (a * 65), 65 * sizeof(long long));
			}
		}
	}

	return ground_bytes;
}

inline float lerp(const float& s, const float& e, const float& t)
{
	return s + (e - s) * t;
}

inline float blerp(const float& c00, const float& c10, const float& c01, const float& c11, const float& tx, const float& ty)
{
	return lerp(lerp(c00, c10, tx), lerp(c01, c11, tx), ty);
}

inline float GetHeightPoint(
	const std::vector<float>& vec,
	const int& vec_width,
	const int& vec_height,
	const int& gridSzX,
	const int& gridSzY,
	const float& x,
	const float& y
)
{
	const float gx = x / float(vec_height) * gridSzX;
	const float gy = y / float(vec_width) * gridSzY;

	const int gxi = int(gx);
	const int gyi = int(gy);

	const float& c00 = vec[(gxi    ) + (gyi    ) * (gridSzX + 1)];
	const float& c10 = vec[(gxi + 1) + (gyi    ) * (gridSzX + 1)];
	const float& c01 = vec[(gxi    ) + (gyi + 1) * (gridSzX + 1)];
	const float& c11 = vec[(gxi + 1) + (gyi + 1) * (gridSzX + 1)];

	return blerp(c00, c10, c01, c11, gx - gxi, gy - gyi);
}

inline glm::vec3 CalculateNormalVector(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
	//p2 is the new origin (can be changed later)
	const glm::vec3 a = p2 - p1;
	const glm::vec3 b = p3 - p1;

	return glm::normalize(glm::cross(a, b));
}

Model* Tile::GenerateTerrainMesh(const std::vector<float>& height_map) const
{
	DebugOutL("Generating terrain mesh...");

	Model* tMesh = new Model();

	const std::size_t tWidth  = static_cast<std::size_t>(m_Width)  * 32 + 1;
	const std::size_t tHeight = static_cast<std::size_t>(m_Height) * 32 + 1;

	const float hWidth  = static_cast<float>(m_Width) * 32.0f;
	const float hHeight = static_cast<float>(m_Height) * 32.0f;

	const bool l_ExportNormals = SharedConverterSettings::ExportNormals;
	const bool l_ExportUvs     = SharedConverterSettings::ExportUvs;

	std::vector<std::size_t> normal_div = {};
	if (l_ExportNormals)
	{
		tMesh->normals.reserve(tWidth * tHeight);
		normal_div.reserve(tWidth * tHeight);
	}

	tMesh->vertices.reserve(tWidth * tHeight);
	for (std::size_t y = 0; y < tHeight; y++)
	{
		for (std::size_t x = 0; x < tWidth; x++)
		{
			const float& height = height_map[x + y * tWidth];
			const float vert_x = -(static_cast<float>(x) * 2.0f) + hWidth;
			const float vert_y = -(static_cast<float>(y) * 2.0f) + hHeight;

			tMesh->vertices.push_back(glm::vec3(vert_x, vert_y, height));

			if (l_ExportNormals)
			{
				tMesh->normals.push_back(glm::vec3(0.0f));
				normal_div.push_back(1);
			}
		}
	}

	if (l_ExportUvs)
	{
		const float uvWidth = static_cast<float>(tWidth - 1);
		const float uvHeight = static_cast<float>(tHeight - 1);

		tMesh->uvs.reserve(tWidth * tHeight);
		for (std::size_t y = 0; y < tHeight; y++)
		{
			for (std::size_t x = 0; x < tWidth; x++)
			{
				const float u = static_cast<float>(x) / uvWidth;
				const float v = static_cast<float>(y) / uvHeight;

				tMesh->uvs.push_back(glm::vec2(u, v));
			}
		}
	}

	SubMeshData* pSubMesh = new SubMeshData(0);

	pSubMesh->has_normals = SharedConverterSettings::ExportNormals;
	pSubMesh->has_uvs     = SharedConverterSettings::ExportUvs;

	//generate normals
	pSubMesh->m_DataIdx.reserve(tWidth * tHeight);
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

			const VertexData vert1 = { h00, l_ExportUvs ? h00 : 0, l_ExportNormals ? h00 : 0 };
			const VertexData vert2 = { h01, l_ExportUvs ? h01 : 0, l_ExportNormals ? h01 : 0 };
			const VertexData vert3 = { h10, l_ExportUvs ? h10 : 0, l_ExportNormals ? h10 : 0 };
			const VertexData vert4 = { h11, l_ExportUvs ? h11 : 0, l_ExportNormals ? h11 : 0 };

			const std::vector<VertexData> pVertData1 = { vert1, vert3, vert2 };
			const std::vector<VertexData> pVertData2 = { vert2, vert3, vert4 };

			pSubMesh->m_DataIdx.push_back(pVertData1);
			pSubMesh->m_DataIdx.push_back(pVertData2);
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

void Tile::WriteTerrain(std::ofstream& model, WriterOffsetData& mOffset, const std::vector<float>& height_map) const
{
	if (m_Type != 0) return;

	DebugOutL("Writing terrain...");
	ProgCounter::SetState(ProgState::WritingGroundMesh, 0);

	//Used to generate the material for ground terrain mesh
	SMGroundTerrainData* v_tempTerrainData = new SMGroundTerrainData();

	Model* terrain = this->GenerateTerrainMesh(height_map);
	terrain->WriteToFile(glm::mat4(1.0f), mOffset, model, v_tempTerrainData);

	delete terrain;
	delete v_tempTerrainData;
}

void Tile::WriteClutter(std::ofstream& model, WriterOffsetData& mOffset, const std::vector<float>& height_map) const
{
	if (m_Type != 0 || !TileConverterSettings::ExportClutter) return;

	DebugOutL("Writing clutter...");

	std::vector<SMTileClutter*> tile_clutter = this->GetClutter();

	const int clWidth  = m_Width  * 128;
	const int clHeight = m_Height * 128;

	const float clWidthF  = static_cast<float>(clWidth);
	const float clHeightF = static_cast<float>(clHeight);

	const int gridSizeX = m_Width  * 32;
	const int gridSizeY = m_Height * 32;

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

			glm::vec3 tClutterPos;
			tClutterPos.x = -((float)x * 0.5f) + tWidth + x_offset;
			tClutterPos.y = -((float)y * 0.5f) + tHeight + y_offset;
			tClutterPos.z = GetHeightPoint(height_map, clWidth, clHeight, gridSizeX, gridSizeY, xPosClamp, yPosClamp);

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

	for (const TilePart* tPart : m_Tiles)
		output += tPart->GetAmountOfObjects();

	return output;
}

void Tile::WriteAssets(std::ofstream& model, WriterOffsetData& mOffset) const
{
	DebugOutL("Writing assets...");

	ProgCounter::SetState(ProgState::WritingObjects, this->GetAmountOfObjects());
	for (int y = 0; y < m_Width; y++)
	{
		for (int x = 0; x < m_Height; x++)
		{
			TilePart* tPart = this->GetPart(x, y);

			tPart->WriteToFile(model, mOffset, x, y);
		}
	}
}

void Tile::WriteMaterials(const std::wstring& dir) const
{
	if (m_Type != 0 || !SharedConverterSettings::ExportMaterials) return;

	DebugOutL("Writing materials...");
	ProgCounter::SetState(ProgState::WritingMaterialMaps, 0);

	const std::vector<long long> ground_data = this->GetGround();

	const int gnd_width  = m_Width  * 64 + 1;
	const int gnd_height = m_Height * 64 + 1;

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
				const long long& cur_data = ground_data[x + y * gnd_width];
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
	if (m_Type != 0 || !SharedConverterSettings::ExportMaterials) return;

	DebugOutL("Writing color map...");
	ProgCounter::SetState(ProgState::WritingColorMap, 0);

	std::vector<int> vert_colors = this->GetVertexColor();

	const int col_width  = m_Width  * 32 + 1;
	const int col_height = m_Height * 32 + 1;

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
			const int& cur_data = vert_colors[x + y * col_width];

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

void Tile::SampleTextures(GroundTexture* tex1, GroundTexture* out_tex, const std::vector<float>& material_map, const int& gnd_width, const int& gnd_height) const
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

	int v_widthDiv = std::max(m_Width / 2, 1);
	int v_heightDiv = std::max(m_Height / 2, 1);
	if (!TileConverterSettings::Export8kGroundTextures)
	{
		v_widthDiv = m_Width * 2;
		v_heightDiv = m_Height * 2;
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

	const std::size_t gnd_width  = static_cast<std::size_t>(m_Width ) * 64 + 1;
	const std::size_t gnd_height = static_cast<std::size_t>(m_Height) * 64 + 1;

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
				const long long& cur_data = ground_data[x + y * gnd_width];
				const Byte cur_chunk = static_cast<Byte>(cur_data >> mat_offset);

				material_vec[x + y * gnd_width] = static_cast<float>(cur_chunk) / 255.0f;

				if (cur_chunk != 0)
					v_curMaterial.HasMatData = true;
			}
		}
	}
}

static const std::wstring GroundTextureNames[] = { L"Dif", L"Asg", L"Nor" };
void Tile::WriteGroundTextures(const std::wstring& dir) const
{
	if (m_Type != 0 || !TileConverterSettings::ExportGroundTextures) return;

	std::array<MaterialData, 8> v_materialMap = {};
	this->FillMaterialMap(v_materialMap);

	const int gnd_width2  = m_Width  * 64;
	const int gnd_height2 = m_Height * 64;

	const int v_gndTexResolution = TileConverterSettings::Export8kGroundTextures ? 8192 : 4096;

	int v_widthDiv = std::max(m_Width / 2, 1);
	int v_heightDiv = std::max(m_Height / 2, 1);
	if (!TileConverterSettings::Export8kGroundTextures)
	{
		v_widthDiv = m_Width * 2;
		v_heightDiv = m_Height * 2;
	}

	for (std::size_t texture_id = 0; texture_id < 3; texture_id++)
	{
		const std::size_t writing_gnd_idx = static_cast<std::size_t>(ProgState::FillingGndDif) + (3 * texture_id);

		GroundTexture gnd_tex;
		if (!gnd_tex.AllocateMemory(v_gndTexResolution, v_gndTexResolution))
			continue;

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

void Tile::WriteToFile(const std::wstring& dir_path, const std::wstring& file_name) const
{
	const std::wstring output_path = dir_path + file_name + L".obj";

	std::ofstream output_model(output_path);
	if (output_model.is_open())
	{
		const std::wstring mtl_name = file_name + L".mtl";
		const std::wstring mtl_path = dir_path + mtl_name;

		if (SharedConverterSettings::ExportMaterials)
		{
			const std::string mtl_header = "mtllib " + String::ToUtf8(mtl_name) + "\n";
			output_model.write(mtl_header.c_str(), mtl_header.size());
		}

		{
			WriterOffsetData offset_data;

			{
				const std::vector<float> pHeightArray = this->GetVertexHeight();

				this->WriteTerrain(output_model, offset_data, pHeightArray);
				this->WriteClutter(output_model, offset_data, pHeightArray);
			}

			this->WriteAssets(output_model, offset_data);

			output_model.close();
		}

		this->WriteMtlFile(mtl_path);

		this->WriteMaterials(dir_path);
		this->WriteColorMap (dir_path);
		this->WriteGroundTextures(dir_path);
	}

	DebugOutL("Finished!");
}

void Tile::WriteMtlFile(const std::wstring& path) const
{
	if (!SharedConverterSettings::ExportMaterials) return;

	DebugOutL("Writing an mtl file...");
	ProgCounter::SetState(ProgState::WritingMtlFile, 0);

	std::unordered_map<std::string, ObjectTexData> tData = {};

	for (const TilePart* tPart : m_Tiles)
	{
		tPart->FillTextureMap(tData);
		ProgCounter::ProgressMax = tData.size();
	}

	{
		ObjectTexData v_tileGroundTextureData;
		v_tileGroundTextureData.TexColor = 0xffffff;

		if (TileConverterSettings::ExportGroundTextures)
		{
			v_tileGroundTextureData.Textures.dif = L"./GroundTexture_Dif.jpg";
			v_tileGroundTextureData.Textures.asg = L"./GroundTexture_Asg.jpg";
			v_tileGroundTextureData.Textures.nor = L"./GroundTexture_Nor.jpg";
		}

		tData["TileGroundTerrain"] = v_tileGroundTextureData;
	}

	MtlFileWriter::Write(path, tData);
}
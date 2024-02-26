#pragma once

#include "TilePart.hpp"

#include "UStd/UnmanagedVector.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

class GroundTexture;
class TileHeader;

class Tile
{
	friend class TileReader;

	int m_Version;
	bool m_hasTerrain;
	long long m_CreatorId;
	int m_Width;
	int m_Height;
	std::vector<TilePart*> m_Tiles;

public:
	Tile(int width, int height);
	Tile(const TileHeader& header);
	~Tile();

	inline int GetWidth() const noexcept { return m_Width; }
	inline int GetHeight() const noexcept { return m_Height; }

	inline void SetPart(int x, int y, TilePart* part)
	{
		assert(part == nullptr);
		m_Tiles[x + y * m_Width] = part;
	}

	inline TilePart* GetPart(int x, int y) const
	{
		return m_Tiles[x + y * m_Width];
	}

	inline TilePart* GetPartSafe(int x, int y) const
	{
		if (x >= m_Width || y >= m_Height)
			return nullptr;

		return m_Tiles[x + y * m_Width];
	}

	void Resize(int width, int height);

	std::vector<float> GetVertexHeight() const;
	std::vector<int> GetVertexColor() const;
	std::vector<SMTileClutter*> GetClutter() const;
	std::vector<long long> GetGround() const;

private:
	Model* GenerateTerrainMesh(const std::vector<float>& height_map) const;
	void WriteTerrain(std::ofstream& model, WriterOffsetData& mOffset, const std::vector<float>& height_map) const;
	void WriteClutter(std::ofstream& model, WriterOffsetData& mOffset, const std::vector<float>& height_map) const;
	std::size_t GetAmountOfObjects() const;
	void WriteAssets(std::ofstream& model, WriterOffsetData& mOffset) const;
	void WriteMaterials(const std::wstring& dir) const;
	void WriteColorMap(const std::wstring& dir) const;

	void FillGndTexture(GroundTexture* mGndTex, const std::size_t& tex_id) const;
	void FillMaterialMap(std::array<struct MaterialData, 8>& mat_data) const;
	void SampleTextures(GroundTexture* tex1, GroundTexture* out_tex, const std::vector<float>& material_map, int gnd_width, int gnd_height) const;
	void WriteGroundTextures(const std::wstring& dir) const;

public:
	void WriteToFile(const std::wstring& dir_path, const std::wstring& file_name) const;
	void WriteMtlFile(const std::wstring& path) const;
};

SM_MANAGED_CODE
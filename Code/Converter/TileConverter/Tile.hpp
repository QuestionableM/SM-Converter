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

	int m_version;
	bool m_hasTerrain;
	std::uint64_t m_creatorId;
	int m_width;
	int m_height;
	std::vector<TilePart*> m_tiles;

public:
	Tile();
	Tile(const TileHeader& header);
	Tile(Tile&& other) noexcept;
	~Tile();

	void operator=(Tile&& other) noexcept;

	inline int GetWidth() const noexcept { return m_width; }
	inline int GetHeight() const noexcept { return m_height; }

	void SetPart(int x, int y, TilePart* part);
	TilePart* GetPart(int x, int y) const;
	TilePart* GetPartSafe(int x, int y) const noexcept;

	void Resize(int width, int height);

	std::vector<float> GetVertexHeight() const;
	std::vector<int> GetVertexColor() const;
	std::vector<SMTileClutter*> GetClutter() const;
	std::vector<long long> GetGround() const;

private:
	void GenerateTerrainMesh(Model& outModel, const std::vector<float>& height_map) const;
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
	void WriteModel(const std::wstring& file_path, const std::wstring& mtl_name) const;
	void WriteToFile(const std::wstring& dir_path, const std::wstring& file_name) const;
	void WriteMtlFile(const std::wstring& path) const;
};

SM_MANAGED_CODE
#pragma once

#include "TilePart.hpp"

#include "UStd\UnmanagedVector.hpp"

#pragma unmanaged

class GroundTexture;

class Tile
{
	int m_Version;
	int m_Type;
	long long m_CreatorId;
	int m_Width;
	int m_Height;
	std::vector<TilePart*> m_Tiles;

public:
	Tile(const int& width, const int& height);
	~Tile();

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	inline int GetVersion() const { return m_Version; }
	//UUID IMPLEMENTATION GOES HERE
	inline int GetTileType() const { return m_Type; }
	inline long long GetCreatorId() const { return m_CreatorId; }

	inline void SetVersion(const int& version) { this->m_Version = version; }
	inline void SetTileType(const int& type) { this->m_Type = type; }
	inline void SetCreatorId(const long long& creator_id) { this->m_CreatorId = creator_id; }

	inline void SetPart(const int& x, const int& y, TilePart* part)
	{
		assert(part == nullptr);
		m_Tiles[x + y * m_Width] = part;
	}

	inline TilePart* GetPart(const int& x, const int& y) const
	{
		return m_Tiles[x + y * m_Width];
	}

	void Resize(const int& width, const int& height);

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
	void SampleTextures(GroundTexture* tex1, GroundTexture* out_tex, const std::vector<float>& material_map, const int& gnd_width, const int& gnd_height) const;
	void WriteGroundTextures(const std::wstring& dir) const;

public:
	void WriteToFile(const std::wstring& dir_path, const std::wstring& file_name) const;
	void WriteMtlFile(const std::wstring& path) const;
};

#pragma managed
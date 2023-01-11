#pragma once

#include "Utils\WinInclude.hpp"
#include "Utils\ByteImpl.hpp"
#include "Utils\Json.hpp"

#include <vector>
#include <string>
#include <array>

#include <FreeImage.h>

class GroundTexture
{
	int m_width, m_height;
	std::wstring m_texturePath;

	FIBITMAP* m_imageData = nullptr;

public:
	GroundTexture() = default;
	GroundTexture(const std::wstring& path);
	GroundTexture(const GroundTexture&) = delete;
	GroundTexture(GroundTexture&&) = delete;
	GroundTexture(GroundTexture&) = delete;
	~GroundTexture();

	bool AllocateMemory(const int& xSize, const int& ySize);
	void Resize(const int& width, const int& height);
	bool LoadImageData();
	void Clear();

	void SetPath(const std::wstring& path);

	inline int GetWidth() const { return m_width; }
	inline int GetHeight() const { return m_height; }

	inline FIBITMAP* Data() { return m_imageData; };
	inline void SetData(FIBITMAP* p_tex_data) { m_imageData = p_tex_data; }

	inline void GetByte(const int& p_x, const int& p_y, RGBQUAD* p_pixel) const
	{
		FreeImage_GetPixelColor(m_imageData, p_x, p_y, p_pixel);
	}

	inline void SetByte(const int& p_x, const int& p_y, RGBQUAD* new_data)
	{
		FreeImage_SetPixelColor(m_imageData, p_x, p_y, new_data);
	}

	void WriteToFile(const std::wstring& path, const int& quality = 90) const;
};

using GroundTexBundle = std::array<GroundTexture*, 3>;

class GroundTextureDatabase
{
	static GroundTexBundle DefaultTex;
	static std::array<GroundTexBundle, 8> TexStorage;

	GroundTextureDatabase() = default;

	static void LoadTextureData(const simdjson::dom::element& tObj, GroundTexBundle& tBundle);
public:
	static void Initialize();
	static void ClearTextureDatabase();

	static GroundTexture* GetDefaultTexture(const std::size_t& index);
	static GroundTexture* GetTexture(const std::size_t& index, const std::size_t& type);
};
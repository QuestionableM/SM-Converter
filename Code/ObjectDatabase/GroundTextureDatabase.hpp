#pragma once

#include "UStd\UnmanagedString.hpp"
#include "UStd\UnmanagedArray.hpp"

#include "Utils\WinInclude.hpp"
#include "Utils\ByteImpl.hpp"
#include "Utils\Json.hpp"

#pragma unmanaged

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
	~GroundTexture();

	bool AllocateMemory(int xSize, int ySize);
	void Resize(int width, int height);
	bool LoadImageData();
	void Clear();

	void SetPath(const std::wstring& path);

	inline int GetWidth() const noexcept { return m_width; }
	inline int GetHeight() const noexcept { return m_height; }

	inline FIBITMAP* Data() noexcept { return m_imageData; };
	inline void SetData(FIBITMAP* p_tex_data) noexcept { m_imageData = p_tex_data; }

	inline void GetByte(int p_x, int p_y, RGBQUAD* p_pixel) const
	{
		FreeImage_GetPixelColor(m_imageData, p_x, p_y, p_pixel);
	}

	inline void SetByte(int p_x, int p_y, RGBQUAD* new_data)
	{
		FreeImage_SetPixelColor(m_imageData, p_x, p_y, new_data);
	}

	void WriteToFile(const std::wstring& path, int quality = 90) const;
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

	static GroundTexture* GetDefaultTexture(std::size_t index);
	static GroundTexture* GetTexture(std::size_t index, std::size_t type);
};

#pragma managed
#pragma once

#include "UStd/UnmanagedString.hpp"
#include "UStd/UnmanagedArray.hpp"

#include "Utils/clr_include.hpp"
#include "Utils/WinInclude.hpp"
#include "Utils/ByteImpl.hpp"
#include "Utils/Json.hpp"

SM_UNMANAGED_CODE

class GroundTexture
{
	int m_width, m_height;
	std::wstring m_texturePath;

	struct FIBITMAP* m_imageData = nullptr;

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

	void SetPath(const std::wstring_view& path);

	int GetWidth() const noexcept;
	int GetHeight() const noexcept;

	FIBITMAP* GetData() noexcept;
	void SetData(FIBITMAP* pTexData) noexcept;

	void GetByte(const int x, const int y, RGBQUAD* pOut) const;
	void SetByte(const int x, const int y, RGBQUAD* pByte);

	bool WriteToFile(const std::wstring_view& path, const int quality = 90) const;
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

	static GroundTexture* GetDefaultTexture(const std::size_t index);
	static GroundTexture* GetTexture(const std::size_t index, const std::size_t type);
};

SM_MANAGED_CODE
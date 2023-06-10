#pragma once

#include "UStd\UnmanagedString.hpp"

#pragma unmanaged

enum TileSizeFilter : unsigned char
{
	TileSizeFilter_Small      = (1 << 0),
	TileSizeFilter_Medium     = (1 << 1),
	TileSizeFilter_Large      = (1 << 2),
	TileSizeFilter_ExtraLarge = (1 << 3),

	TileSizeFilter_Any = TileSizeFilter_Small | TileSizeFilter_Medium | TileSizeFilter_Large | TileSizeFilter_ExtraLarge
};

enum UserDataFilter : unsigned char
{
	UserDataFilter_GameItems     = (1 << 0),
	UserDataFilter_WorkshopItems = (1 << 1),
	UserDataFilter_UserItems     = (1 << 2),

	UserDataFilter_Any = UserDataFilter_GameItems | UserDataFilter_WorkshopItems | UserDataFilter_UserItems
};

struct FilterSettingsData
{
	inline static unsigned char TileSizeFilter = TileSizeFilter::TileSizeFilter_Any;
	inline static unsigned char UserDataFilter = UserDataFilter::UserDataFilter_Any;

	static unsigned char GetUserDataFilter(const std::wstring& path);

	inline static wchar_t* GetTileSizeName(unsigned char v_id)
	{
		switch (v_id)
		{
		case TileSizeFilter_Small: return L"Small";
		case TileSizeFilter_Medium: return L"Medium";
		case TileSizeFilter_Large: return L"Large";
		case TileSizeFilter_ExtraLarge: return L"Extra Large";
		}

		return L"UNKNOWN";
	}

	inline static wchar_t* GetFilterName(unsigned char v_id)
	{
		switch (v_id)
		{
		case UserDataFilter_GameItems: return L"Game Item";
		case UserDataFilter_WorkshopItems: return L"Workshop Item";
		case UserDataFilter_UserItems: return L"User Item";
		}

		return L"UNKNOWN";
	}

private:
	FilterSettingsData() = default;
	FilterSettingsData(const FilterSettingsData&) = delete;
	FilterSettingsData(FilterSettingsData&&) = delete;
	~FilterSettingsData() = default;
};

#pragma managed
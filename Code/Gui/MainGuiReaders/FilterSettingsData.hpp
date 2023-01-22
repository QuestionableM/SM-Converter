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

private:
	FilterSettingsData() = default;
	FilterSettingsData(const FilterSettingsData&&) = delete;
	FilterSettingsData(const FilterSettingsData&) = delete;
	FilterSettingsData(FilterSettingsData&) = delete;
	~FilterSettingsData() = default;
};

#pragma managed
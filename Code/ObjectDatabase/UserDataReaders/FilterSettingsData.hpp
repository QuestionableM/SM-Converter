#pragma once

#include "UStd/UnmanagedString.hpp"
#include "Utils/clr_include.hpp"

SM_UNMANAGED_CODE

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

	inline static std::string_view GetTileSizeName(unsigned char v_id)
	{
		switch (v_id)
		{
		case TileSizeFilter_Small: return "Small";
		case TileSizeFilter_Medium: return "Medium";
		case TileSizeFilter_Large: return "Large";
		case TileSizeFilter_ExtraLarge: return "Extra Large";
		}

		return "UNKNOWN";
	}

	inline static std::string_view GetFilterName(unsigned char v_id)
	{
		switch (v_id)
		{
		case UserDataFilter_GameItems: return "Game Item";
		case UserDataFilter_WorkshopItems: return "Workshop Item";
		case UserDataFilter_UserItems: return "User Item";
		}

		return "UNKNOWN";
	}

private:
	FilterSettingsData() = default;
	FilterSettingsData(const FilterSettingsData&) = delete;
	FilterSettingsData(FilterSettingsData&&) = delete;
	~FilterSettingsData() = default;
};

SM_MANAGED_CODE
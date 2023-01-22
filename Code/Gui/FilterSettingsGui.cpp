#include "FilterSettingsGui.h"

#include "MainGuiReaders\FilterSettingsData.hpp"
#include "Utils\Console.hpp"

namespace SMConverter
{
	FilterSettingsGui::FilterSettingsGui()
	{
		this->InitializeComponent();

		switch (FilterSettingsData::TileSizeFilter)
		{
		case TileSizeFilter_Any:        m_cb_tileSize->SelectedIndex = 0; break;
		case TileSizeFilter_Small:      m_cb_tileSize->SelectedIndex = 1; break;
		case TileSizeFilter_Medium:     m_cb_tileSize->SelectedIndex = 2; break;
		case TileSizeFilter_Large:      m_cb_tileSize->SelectedIndex = 3; break;
		case TileSizeFilter_ExtraLarge: m_cb_tileSize->SelectedIndex = 4; break;
		}

		m_cb_showGameItems->Checked = (FilterSettingsData::UserDataFilter & UserDataFilter_GameItems);
		m_cb_showUserItems->Checked = (FilterSettingsData::UserDataFilter & UserDataFilter_UserItems);
		m_cb_showWorkshopItems->Checked = (FilterSettingsData::UserDataFilter & UserDataFilter_WorkshopItems);

		this->m_cb_tileSize->SelectedIndexChanged += gcnew System::EventHandler(this, &FilterSettingsGui::Filter_TileSizeFilter_SelectedIndexChanged);
		this->m_cb_showWorkshopItems->CheckedChanged += gcnew System::EventHandler(this, &FilterSettingsGui::Filter_ShowWorkshopItems_CheckedChanged);
		this->m_cb_showGameItems->CheckedChanged += gcnew System::EventHandler(this, &FilterSettingsGui::Filter_ShowGameItems_CheckedChanged);
		this->m_cb_showUserItems->CheckedChanged += gcnew System::EventHandler(this, &FilterSettingsGui::Filter_ShowUserItems_CheckedChanged);

		m_tileSizeFilterCopy = FilterSettingsData::TileSizeFilter;
		m_userDataFilterCopy = FilterSettingsData::UserDataFilter;
	}

	FilterSettingsGui::~FilterSettingsGui()
	{
		if (components) delete components;
	}

	inline void SetBit(unsigned char& v_bit_ref, const unsigned char& v_value, const bool& is_set)
	{
		if (is_set)
			v_bit_ref |= v_value;
		else
			v_bit_ref &= ~v_value;
	}

	void FilterSettingsGui::Filter_ShowWorkshopItems_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		SetBit(FilterSettingsData::UserDataFilter, UserDataFilter_WorkshopItems, m_cb_showWorkshopItems->Checked);
		this->UpdateChangedStatus();
	}

	void FilterSettingsGui::Filter_ShowGameItems_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		SetBit(FilterSettingsData::UserDataFilter, UserDataFilter_GameItems, m_cb_showGameItems->Checked);
		this->UpdateChangedStatus();
	}

	void FilterSettingsGui::Filter_ShowUserItems_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		SetBit(FilterSettingsData::UserDataFilter, UserDataFilter_UserItems, m_cb_showUserItems->Checked);
		this->UpdateChangedStatus();
	}

	void FilterSettingsGui::Filter_TileSizeFilter_SelectedIndexChanged(System::Object ^ sender, System::EventArgs ^ e)
	{
		const static unsigned char v_filters[] =
		{
			TileSizeFilter_Any,
			TileSizeFilter_Small,
			TileSizeFilter_Medium,
			TileSizeFilter_Large,
			TileSizeFilter_ExtraLarge
		};

		FilterSettingsData::TileSizeFilter = v_filters[m_cb_tileSize->SelectedIndex];
		this->UpdateChangedStatus();
	}

	void FilterSettingsGui::UpdateChangedStatus()
	{
		m_shouldReload =
			(FilterSettingsData::UserDataFilter != m_userDataFilterCopy) ||
			(FilterSettingsData::TileSizeFilter != m_tileSizeFilterCopy);
	}
}
#include "MainGui.h"
#include "AboutGui.h"

#include "Utils\Console.hpp"
#include "Utils\File.hpp"

#include <Windows.h>
#include <WinUser.h>
#include <CommCtrl.h>

enum
{
	Generator_BlueprintConverter = 0,
	Generator_TileConverter      = 1,
	Generator_ScriptConverter    = 2
};

namespace SMConverter
{
	MainGui::MainGui()
	{
		this->InitializeComponent();

		m_cb_selectedGenerator->SelectedIndex = 0;
	}

	MainGui::~MainGui()
	{
		if (components) delete components;
	}

	void MainGui::SelectedGenerator_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
	{
		//Used by the path text box to show appropriate text based on the selected generator type
		const static wchar_t* g_selectedGeneratorMessages[] =
		{
			L"Path to Blueprint",
			L"Path to Tile",
			L"Path to Script"
		};

		//Used by the search text box to show appropriate text based on the selected generator type
		const static wchar_t* g_searchTextMessages[] =
		{
			L"Search Blueprints",
			L"Search Tiles",
			L"Search Scripts"
		};

		m_tb_searchBox->Clear();

		SendMessage(
			static_cast<HWND>(m_tb_path->Handle.ToPointer()),
			EM_SETCUEBANNER,
			0,
			reinterpret_cast<LPARAM>(g_selectedGeneratorMessages[m_cb_selectedGenerator->SelectedIndex])
		);

		SendMessage(
			static_cast<HWND>(m_tb_searchBox->Handle.ToPointer()),
			EM_SETCUEBANNER,
			0,
			reinterpret_cast<LPARAM>(g_searchTextMessages[m_cb_selectedGenerator->SelectedIndex])
		);
	}

	void MainGui::PathTextBox_TextChanged(System::Object^ sender, System::EventArgs^ e)
	{
		m_btn_convert->Enabled = (m_tb_path->TextLength > 0);
	}

	void MainGui::FolderDialog_Click(System::Object^ sender, System::EventArgs^ e)
	{
		const std::wstring g_fileDialogTitles[] =
		{
			L"Select a Blueprint",
			L"Select a Tile",
			L"Select a Script"
		};

		const std::wstring v_selected_path = File::OpenFileDialog(
			g_fileDialogTitles[m_cb_selectedGenerator->SelectedIndex],
			FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST,
			L"All Files (*.*)\0*.*\0",
			reinterpret_cast<HWND>(this->Handle.ToPointer())
		);

		if (v_selected_path.empty())
			return;

		m_tb_path->Text = gcnew System::String(v_selected_path.c_str());
	}

	void MainGui::AboutButton_Click(System::Object^ sender, System::EventArgs^ e)
	{
		AboutGui^ v_about_gui = gcnew AboutGui();
		v_about_gui->ShowDialog();
	}

	void MainGui::MainGui_Resize(System::Object^ sender, System::EventArgs^ e)
	{
		System::Drawing::Size v_selector_sz = m_lb_objectSelector->Size;
		System::Drawing::Size v_lbl_sz = m_lbl_objSelectorStatus->Size;

		m_lbl_objSelectorStatus->Left = m_lb_objectSelector->Left + (v_selector_sz.Width / 2) - (v_lbl_sz.Width / 2);
		m_lbl_objSelectorStatus->Top = m_lb_objectSelector->Top + (v_selector_sz.Height / 2) - (v_lbl_sz.Height / 2);
	}
}
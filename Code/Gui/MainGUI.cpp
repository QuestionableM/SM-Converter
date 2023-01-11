#include "MainGui.h"

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
}
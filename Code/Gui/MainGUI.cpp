#include "MainGui.h"
#include "AboutGui.h"

#include "ObjectDatabase\KeywordReplacer.hpp"
#include "ObjectDatabase\ObjectDatabase.hpp"
#include "ObjectDatabase\DatabaseConfig.hpp"
#include "ObjectDatabase\ProgCounter.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\File.hpp"

#include <Windows.h>
#include <WinUser.h>
#include <CommCtrl.h>
#include <msclr\marshal_cppstd.h>

#include <filesystem>

enum : unsigned short
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

		DatabaseLoader::InitializeDatabase();
	}

	MainGui::~MainGui()
	{
		if (components) delete components;
	}

	void MainGui::MainGui_Shown(System::Object^ sender, System::EventArgs^ e)
	{
		namespace WForms = System::Windows::Forms;

		DebugOutL("MainGui has been loaded!");

		if (DatabaseConfig::GamePath.empty() || !File::Exists(DatabaseConfig::GamePath))
		{
			const WForms::DialogResult v_dr = WForms::MessageBox::Show(
				"The program couldn't find a path to Scrap Mechanic.\n\nWould you like to specify the path to Scrap Mechanic by yourself?",
				"No Game Path",
				WForms::MessageBoxButtons::YesNo,
				WForms::MessageBoxIcon::Question
			);

			if (v_dr == WForms::DialogResult::Yes)
			{
				DebugOutL("TODO: Make an options gui");
			}
			else
				this->Close();

			return;
		}

		this->LoadObjectDatabase(false);
		this->LoadUserObjects();
	}

	void MainGui::MainGui_UpdatePathTextBox()
	{
		if (m_lb_objectSelector->SelectedIndex == -1)
		{
			//Used by the path text box to show appropriate text based on the selected generator type
			const static wchar_t* g_selectedGeneratorMessages[] =
			{
				L"Path to Blueprint",
				L"Path to Tile",
				L"Path to Script"
			};

			SendMessage(
				static_cast<HWND>(m_tb_path->Handle.ToPointer()),
				EM_SETCUEBANNER,
				0,
				reinterpret_cast<LPARAM>(g_selectedGeneratorMessages[m_cb_selectedGenerator->SelectedIndex])
			);
		}
		else
		{
			const static wchar_t* g_selectedObjectMessages[] =
			{
				L"Selected a Blueprint in the list",
				L"Selected a tile in the list",
				L"Selected a script in the list"
			};

			SendMessage(
				static_cast<HWND>(m_tb_path->Handle.ToPointer()),
				EM_SETCUEBANNER,
				0,
				reinterpret_cast<LPARAM>(g_selectedObjectMessages[m_cb_selectedGenerator->SelectedIndex])
			);
		}
	}

	void MainGui::SelectedGenerator_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
	{
		//Used by the search text box to show appropriate text based on the selected generator type
		const static wchar_t* g_searchTextMessages[] =
		{
			L"Search Blueprints",
			L"Search Tiles",
			L"Search Scripts"
		};

		m_tb_searchBox->Clear();
		
		if (m_obj_isLoaded)
		{
			this->UpdateCurrentObjectList();
		}
		else
		{
			this->UpdateObjectListStatus();
		}

		this->MainGui_UpdatePathTextBox();

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

		if (m_tb_path->TextLength > 0)
			m_lb_objectSelector->SelectedIndex = -1;
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

	void MainGui::MainGui_Timer_UpdateProgressBar(System::Object^ sender, System::EventArgs^ e)
	{
		if (ProgCounter::State == ProgState::None)
			return;

		std::wstring v_state_output = ProgCounter::GetStateString();
		if (ProgCounter::StateHasNumbers())
		{
			const std::size_t v_max_val = ProgCounter::ProgressMax;
			const std::size_t v_cur_val = std::min(ProgCounter::ProgressValue, v_max_val);

			m_pb_progress->Maximum = static_cast<int>(v_max_val);
			m_pb_progress->Value   = static_cast<int>(v_cur_val);

			//Append numbers to the string
			v_state_output.append(L"(");
			v_state_output.append(std::to_wstring(v_cur_val));
			v_state_output.append(L" / ");
			v_state_output.append(std::to_wstring(v_max_val));
			v_state_output.append(L")");
		}

		m_lbl_progressStatus->Text = gcnew System::String(v_state_output.c_str());
	}

	void MainGui::LoadObjectDatabase(const bool& should_reload)
	{
		if (m_bw_databaseLoader->IsBusy) return;

		DebugOutL(0b0011_fg, "Loading object database...");

		this->MainGui_ChangeGuiState(false, m_obj_isLoaded, true);
		m_progressBarUpdater->Start();
		m_bw_databaseLoader->RunWorkerAsync(should_reload);
	}

	void MainGui::MainGui_DatabaseLoader_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e)
	{
		const bool v_should_update = safe_cast<bool>(e->Argument);
		if (v_should_update)
			KeywordReplacer::Initialize();

		DatabaseLoader::LoadDatabase();
	}

	void MainGui::MainGui_DatabaseLoader_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e)
	{
		m_progressBarUpdater->Stop();
		this->MainGui_ChangeGuiState(true, m_obj_isLoaded, true);

		ProgCounter::SetState(ProgState::None, 0);

		m_pb_progress->Value = 0;
		m_pb_progress->Maximum = 0;

		const std::size_t v_mod_count = Mod::GetAmountOfMods();
		const std::size_t v_obj_count = Mod::GetAmountOfObjects();

		const std::wstring v_load_msg = L"Successfully loaded " + std::to_wstring(v_obj_count) + L" objects from " + std::to_wstring(v_mod_count) + L" mods";
		m_lbl_progressStatus->Text = gcnew System::String(v_load_msg.c_str());
	}

	void MainGui::MainGui_ChangeGuiState(bool db_loaded, bool objlist_loaded, bool obj_converted)
	{
		m_database_isLoaded = db_loaded;
		m_obj_isLoaded = objlist_loaded;

		const bool objlist_and_obj_loaded = objlist_loaded && obj_converted;
		const bool db_loaded_and_obj_converted = db_loaded && obj_converted;

		m_tb_path->Enabled = db_loaded_and_obj_converted;
		m_btn_folderDialog->Enabled = db_loaded_and_obj_converted;
		m_tb_searchBox->Enabled = objlist_and_obj_loaded;

		m_btn_reloadUserObjects->Enabled = objlist_and_obj_loaded;
		m_btn_reloadDatabase->Enabled = db_loaded_and_obj_converted;
		m_lb_objectSelector->Enabled = objlist_and_obj_loaded;

		m_btn_convert->Enabled = db_loaded_and_obj_converted && (m_tb_path->TextLength > 0);
		m_btn_options->Enabled = db_loaded_and_obj_converted;
	}

	void MainGui::MainGui_ReloadDatabase_Click(System::Object^ sender, System::EventArgs^ e)
	{
		this->LoadObjectDatabase(true);
	}

	void MainGui::LoadUserObjects()
	{
		if (m_bw_objectLoader->IsBusy) return;

		m_lbl_objSelectorStatus->Visible = true;
		this->UpdateObjectListStatus();

		this->MainGui_ChangeGuiState(m_database_isLoaded, false, true);
		m_bw_objectLoader->RunWorkerAsync();
	}

	void MainGui::UpdateObjectListStatus()
	{
		const static wchar_t* g_objectListStatusMessages[] =
		{
			L"Loading Blueprints...",
			L"Loading Tiles...",
			L"Loading Scripts..."
		};

		const wchar_t* v_cur_message = g_objectListStatusMessages[m_cb_selectedGenerator->SelectedIndex];
		m_lbl_objSelectorStatus->Text = gcnew System::String(v_cur_message);
	}

	void MainGui::UpdateCurrentObjectList()
	{
		m_lbl_objSelectorStatus->Visible = false;

		m_lb_objectSelector->BeginUpdate();
		m_lb_objectSelector->Items->Clear();

		switch (m_cb_selectedGenerator->SelectedIndex)
		{
		case Generator_BlueprintConverter:
			{
				const std::vector<BlueprintInstance*>& v_cur_bp_list = this->GetCurrentBlueprintList();
				if (v_cur_bp_list.empty())
					break;

				for (const BlueprintInstance* v_bp_instance : v_cur_bp_list)
					m_lb_objectSelector->Items->Add(gcnew System::String(v_bp_instance->name.c_str()));

				m_lb_objectSelector->EndUpdate();
				return;
			}
		case Generator_TileConverter:
			{
				const std::vector<TileInstance*>& v_cur_tile_list = this->GetCurrentTileList();
				if (v_cur_tile_list.empty())
					break;

				for (const TileInstance* v_tile_instance : v_cur_tile_list)
					m_lb_objectSelector->Items->Add(gcnew System::String(v_tile_instance->name.c_str()));

				m_lb_objectSelector->EndUpdate();
				return;
			}
		case Generator_ScriptConverter:
			//TODO: IMPLEMENT LATER
			break;
		}

		m_lb_objectSelector->EndUpdate();

		const static wchar_t* g_noObjectMessages[] =
		{
			L"No Blueprints",
			L"No Tiles",
			L"No Scripts"
		};

		const wchar_t* v_cur_message = g_noObjectMessages[m_cb_selectedGenerator->SelectedIndex];
		m_lbl_objSelectorStatus->Text = gcnew System::String(v_cur_message);
		m_lbl_objSelectorStatus->Visible = true;
	}

	void MainGui::UpdateSearchResults()
	{
		//TODO: IMPLEMENT LATER
	}

	void MainGui::MainGui_ObjectLoader_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e)
	{
		//TODO: Make a script loader (not important for now)
		BlueprintFolderReader::ReadBlueprintsFromConfig();
		TileFolderReader::ReadTilesFromConfig();
	}

	void MainGui::MainGui_ObjectLoader_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e)
	{
		this->MainGui_ChangeGuiState(m_database_isLoaded, true, true);
		this->UpdateCurrentObjectList();
	}

	void MainGui::MainGui_ReloadUserObjects_Click(System::Object^ sender, System::EventArgs^ e)
	{
		this->LoadUserObjects();
	}

	void MainGui::MainGui_ObjectSelector_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
	{
		if (m_lb_objectSelector->SelectedIndex >= 0)
		{
			this->MainGui_UpdatePathTextBox();
			m_tb_path->Clear();
		}
	}

	template<typename T>
	inline void SearchFunction(
		const std::wstring& search_str,
		int v_lastSearchLength,
		int v_currentSearchLength)
	{
		if (T::Storage.empty())
			return;

		if (v_lastSearchLength != 0 && v_currentSearchLength > v_lastSearchLength)
		{
			std::size_t v_newCacheSize = 0;

			for (T::InstanceType* v_cur_instance : T::SearchResults)
				if (v_cur_instance->lower_name.find(search_str) != std::wstring::npos)
					T::SearchResults[v_newCacheSize++] = v_cur_instance;

			T::SearchResults.resize(v_newCacheSize);
		}
		else
		{
			T::SearchResults.clear();

			for (T::InstanceType* v_cur_instance : T::Storage)
				if (v_cur_instance->lower_name.find(search_str) != std::wstring::npos)
					T::SearchResults.push_back(v_cur_instance);
		}
	}

	void MainGui::MainGui_SearchBox_TextChanged(System::Object^ sender, System::EventArgs^ e)
	{
		if (m_tb_searchBox->TextLength > 0)
		{
			std::wstring v_searchWstr = msclr::interop::marshal_as<std::wstring>(m_tb_searchBox->Text);
			::String::ToLowerR(v_searchWstr);

			switch (m_cb_selectedGenerator->SelectedIndex)
			{
			case Generator_BlueprintConverter:
				SearchFunction<BlueprintFolderReader>(v_searchWstr, m_lastSearchLength, m_tb_searchBox->TextLength);
				break;
			case Generator_TileConverter:
				SearchFunction<TileFolderReader>(v_searchWstr, m_lastSearchLength, m_tb_searchBox->TextLength);
				break;
			case Generator_ScriptConverter:
				break;
			}
		}

		m_lastSearchLength = m_tb_searchBox->TextLength;
		this->UpdateCurrentObjectList();
	}

	std::vector<BlueprintInstance*>& MainGui::GetCurrentBlueprintList()
	{
		return (m_tb_searchBox->TextLength > 0) ? BlueprintFolderReader::SearchResults : BlueprintFolderReader::Storage;
	}

	std::vector<TileInstance*>& MainGui::GetCurrentTileList()
	{
		return (m_tb_searchBox->TextLength > 0) ? TileFolderReader::SearchResults : TileFolderReader::Storage;
	}
}
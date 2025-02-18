#include "MainGui.h"

#include "BlueprintConvertSettings.h"
#include "TileConvertSettings.h"
#include "FilterSettingsGui.h"
#include "ItemInfoGui.h"
#include "SettingsGui.h"
#include "AboutGui.h"

#include "ObjectDatabase\UserDataReaders\UserObjectFolderReader.hpp"
#include "ObjectDatabase\UserDataReaders\UserCharacterReader.hpp"
#include "ObjectDatabase\GroundTextureDatabase.hpp"
#include "ObjectDatabase\KeywordReplacer.hpp"
#include "ObjectDatabase\ObjectDatabase.hpp"
#include "ObjectDatabase\DatabaseConfig.hpp"
#include "ObjectDatabase\ProgCounter.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Converter\BlueprintConverter\BlueprintConverter.hpp"
#include "Converter\CharacterConverter\CharacterConverter.hpp"
#include "Converter\WorldConverter\WorldConverter.hpp"
#include "Converter\TileConverter\TileConverter.hpp"
#include "Converter\ConvertSettings.hpp"

#include "UStd\UnmanagedFilesystem.hpp"

#include "Utils\WinInclude.hpp"
#include "Utils\Console.hpp"
#include "Utils\String.hpp"
#include "Utils\File.hpp"

#include <WinUser.h>
#include <CommCtrl.h>
#include <msclr\marshal_cppstd.h>

//#define SMC_TILE_TESTER
#if defined(SMC_TILE_TESTER)
#include "Converter\TileConverter\Readers\TileReader.hpp"
#include "ObjectDatabase\Mods\CustomGameSwitch.hpp"
#endif

#define WF_SHOW_WARNING(title, message) \
	System::Windows::Forms::MessageBox::Show( \
		message, title, \
		System::Windows::Forms::MessageBoxButtons::OK, \
		System::Windows::Forms::MessageBoxIcon::Warning \
	)

#define WF_SHOW_ERROR(title, message) \
	System::Windows::Forms::MessageBox::Show( \
		message, title, \
		System::Windows::Forms::MessageBoxButtons::OK, \
		System::Windows::Forms::MessageBoxIcon::Error \
	)

enum : unsigned short
{
	Generator_BlueprintConverter = 0,
	Generator_TileConverter      = 1,
	Generator_WorldConverter     = 2,
	Generator_CharacterConverter = 3
};

namespace SMConverter
{
	MainGui::MainGui()
	{
		this->InitializeComponent();

	#if defined(DEBUG) || defined(_DEBUG)
		this->Text += " (DEBUG)";
	#endif

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
				SettingsGui^ v_settings = gcnew SettingsGui();
				this->MainGui_CenterChildForm(v_settings);
				v_settings->ShowDialog();
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
				L"Path to World",
				L"Path to Character"
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
				L"Selected a Tile in the list",
				L"Selected a World in the list",
				L"Selected a Character in the list"
			};

			SendMessage(
				static_cast<HWND>(m_tb_path->Handle.ToPointer()),
				EM_SETCUEBANNER,
				0,
				reinterpret_cast<LPARAM>(g_selectedObjectMessages[m_cb_selectedGenerator->SelectedIndex])
			);
		}
	}

	void MainGui::MainGui_UpdatePathListContextMenuStrip()
	{
		m_cms_blueprint->Close();
		m_cms_tile->Close();
		m_cms_world->Close();

		switch (m_cb_selectedGenerator->SelectedIndex)
		{
		case Generator_BlueprintConverter:
			m_lb_objectSelector->ContextMenuStrip = m_cms_blueprint;
			break;
		case Generator_TileConverter:
			m_lb_objectSelector->ContextMenuStrip = m_cms_tile;
			break;
		case Generator_WorldConverter:
			m_lb_objectSelector->ContextMenuStrip = m_cms_world;
			break;
		case Generator_CharacterConverter:
			m_lb_objectSelector->ContextMenuStrip = nullptr;
			break;
		}
	}

	void MainGui::SelectedGenerator_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
	{
		//Used by the search text box to show appropriate text based on the selected generator type
		const static wchar_t* g_searchTextMessages[] =
		{
			L"Search Blueprints",
			L"Search Tiles",
			L"Search Worlds",
			L"Search Characters"
		};

		SendMessage(
			static_cast<HWND>(m_tb_searchBox->Handle.ToPointer()),
			EM_SETCUEBANNER,
			0,
			reinterpret_cast<LPARAM>(g_searchTextMessages[m_cb_selectedGenerator->SelectedIndex])
		);

		if (m_obj_isLoaded)
			this->UpdateSearchResults(0);
		else
			this->UpdateObjectListStatus();

		this->UpdateConvertButton();
		this->UpdateContextMenuStrip();
		this->MainGui_UpdatePathTextBox();
		this->MainGui_UpdatePathListContextMenuStrip();
	}

	void MainGui::PathTextBox_TextChanged(System::Object^ sender, System::EventArgs^ e)
	{
		this->UpdateConvertButton();

		if (m_tb_path->TextLength > 0)
			m_lb_objectSelector->SelectedIndex = -1;
	}

	void MainGui::FolderDialog_Click(System::Object^ sender, System::EventArgs^ e)
	{
		const std::wstring g_fileDialogTitles[] =
		{
			L"Select a Blueprint",
			L"Select a Tile",
			L"Select a World",
			L"Select a Character"
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
		this->MainGui_CenterChildForm(v_about_gui);
		v_about_gui->ShowDialog();
		
	#if defined(SMC_TILE_TESTER)
		std::size_t v_successfull_loads = 0;
		for (const TileInstance* v_tile_instance : TileFolderReader::Storage)
		{
			ConvertError v_error;

			Tile* v_cur_tile = nullptr;
			CustomGame* v_current_cg = SMMod::GetCustomGameFromPath(v_tile_instance->directory);
			if (v_current_cg)
			{
				SMModCustomGameSwitch<false, true> v_cg_switch;
				v_cg_switch.MergeContent(v_current_cg);

				v_cur_tile = TileReader::ReadTile<true>(v_tile_instance->path, v_error);
			}
			else
			{
				v_cur_tile = TileReader::ReadTile<true>(v_tile_instance->path, v_error);
			}

			if (v_cur_tile)
				delete v_cur_tile;

			if (v_error)
			{
				DebugErrorL("Failed to load tile: ", v_tile_instance->path);
				DebugErrorL("Error: ", v_error.GetErrorMsg(), "\n");
			}
			else
			{
				v_successfull_loads++;
			}
		}

		DebugOutL("Self-Diagnosis finished! Loaded: ", v_successfull_loads, "/", TileFolderReader::Storage.size(), " tiles");
	#endif
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

		std::string v_stateOutput(ProgCounter::GetStateString());
		if (ProgCounter::StateHasNumbers())
		{
			const std::size_t v_max_val = ProgCounter::ProgressMax.load();
			const std::size_t v_cur_val = std::min(ProgCounter::ProgressValue.load(), v_max_val);

			m_pb_progress->Maximum = static_cast<int>(v_max_val);
			m_pb_progress->Value   = static_cast<int>(v_cur_val);

			// Append numbers to the string
			v_stateOutput.append("(");
			v_stateOutput.append(std::to_string(v_cur_val));
			v_stateOutput.append(" / ");
			v_stateOutput.append(std::to_string(v_max_val));
			v_stateOutput.append(")");
		}

		m_lbl_progressStatus->Text = gcnew System::String(v_stateOutput.c_str());
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
		this->ResetProgressBar();

		const std::size_t v_mod_count = SMMod::GetAmountOfMods();
		const std::size_t v_obj_count = SMMod::GetAmountOfObjects();

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
		m_btn_searchFilter->Enabled = objlist_and_obj_loaded;

		m_btn_reloadUserObjects->Enabled = objlist_and_obj_loaded;
		m_btn_reloadDatabase->Enabled = db_loaded_and_obj_converted;
		m_lb_objectSelector->Enabled = objlist_and_obj_loaded;

		m_btn_convert->Enabled = db_loaded_and_obj_converted && (m_tb_path->TextLength > 0 || m_lb_objectSelector->SelectedIndex >= 0);
		m_btn_options->Enabled = db_loaded_and_obj_converted;

		m_cb_selectedGenerator->Enabled = obj_converted;
		m_menuStrip->Enabled = obj_converted;

		m_cms_blueprint->Enabled = objlist_and_obj_loaded;
		m_btn_openBlueprintFolder->Enabled = false;
		m_btn_openBlueprintInSteamWorkshop->Enabled = false;

		m_cms_tile->Enabled = objlist_and_obj_loaded;
		m_btn_openTileInSteamWorkshop->Enabled = false;
		m_btn_openTileFolder->Enabled = false;
		m_btn_findTileCreatorInSteam->Enabled = false;

		if (objlist_and_obj_loaded)
			this->UpdateContextMenuStrip();
	}

	void MainGui::MainGui_ReloadDatabase_Click(System::Object^ sender, System::EventArgs^ e)
	{
		this->LoadObjectDatabase(true);
	}

	void MainGui::LoadUserObjects()
	{
		if (m_bw_objectLoader->IsBusy) return;

		m_lbl_objSelectorStatus->Visible = true;
		m_lb_objectSelector->SelectedIndex = -1;

		this->UpdateObjectListStatus();
		this->MainGui_UpdatePathTextBox();

		this->MainGui_ChangeGuiState(m_database_isLoaded, false, true);
		m_bw_objectLoader->RunWorkerAsync();
	}

	void MainGui::UpdateObjectListStatus()
	{
		const static wchar_t* g_objectListStatusMessages[] =
		{
			L"Loading Blueprints...",
			L"Loading Tiles...",
			L"Loading Worlds...",
			L"Loading Characters..."
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
		case Generator_WorldConverter:
			{
				const std::vector<WorldInstance*>& v_cur_world_list = this->GetCurrentWorldList();
				if (v_cur_world_list.empty())
					break;

				for (const WorldInstance* v_world_instance : v_cur_world_list)
					m_lb_objectSelector->Items->Add(gcnew System::String(v_world_instance->name.c_str()));

				m_lb_objectSelector->EndUpdate();
				return;
			}
		case Generator_CharacterConverter:
			{
				const std::vector<UserCharacterInstance*>& v_cur_char_list = this->GetCurrentCharacterList();
				if (v_cur_char_list.empty())
					break;

				for (const UserCharacterInstance* v_user_char_instance : v_cur_char_list)
					m_lb_objectSelector->Items->Add(gcnew System::String(v_user_char_instance->name.c_str()));

				m_lb_objectSelector->EndUpdate();
				return;
			}
		}

		m_lb_objectSelector->EndUpdate();

		const static wchar_t* g_noObjectMessages[] =
		{
			L"No Blueprints",
			L"No Tiles",
			L"No Worlds",
			L"No Characters"
		};

		const wchar_t* v_cur_message = g_noObjectMessages[m_cb_selectedGenerator->SelectedIndex];
		m_lbl_objSelectorStatus->Text = gcnew System::String(v_cur_message);
		m_lbl_objSelectorStatus->Visible = true;
	}

	void MainGui::MainGui_ObjectLoader_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e)
	{
		UserObjectFolderReader::ReadFromConfig();
	}

	void MainGui::MainGui_ObjectLoader_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e)
	{
		this->UpdateSearchResults(0);

		this->MainGui_ChangeGuiState(m_database_isLoaded, true, true);
	}

	void MainGui::MainGui_ReloadUserObjects_Click(System::Object^ sender, System::EventArgs^ e)
	{
		this->LoadUserObjects();
	}

	void MainGui::MainGui_ObjectSelector_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		if (e->Button != System::Windows::Forms::MouseButtons::Right) return;

		const int v_idx = m_lb_objectSelector->IndexFromPoint(e->X, e->Y);
		if (v_idx == -1) return;

		m_lb_objectSelector->SetSelected(v_idx, true);
	}

	void MainGui::MainGui_ObjectSelector_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
	{
		this->UpdateConvertButton();
		this->UpdateContextMenuStrip();

		if (m_lb_objectSelector->SelectedIndex >= 0)
		{
			this->MainGui_UpdatePathTextBox();
			m_tb_path->Clear();
		}
	}

	inline void FilterStorageIfNeeded(int selected_index)
	{
		switch (selected_index)
		{
		case Generator_BlueprintConverter:
			BlueprintFolderReader::UpdateStorage();
			break;
		case Generator_TileConverter:
			TileFolderReader::UpdateStorage();
			break;
		case Generator_WorldConverter:
			WorldFolderReader::UpdateStorage();
			break;
		case Generator_CharacterConverter:
			UserCharacterReader::UpdateStorage();
			break;
		}
	}

	void MainGui::UpdateSearchResults(int last_search_length)
	{
		FilterStorageIfNeeded(m_cb_selectedGenerator->SelectedIndex);

		if (m_tb_searchBox->TextLength > 0)
		{
			std::wstring v_search_str = msclr::interop::marshal_as<std::wstring>(m_tb_searchBox->Text);
			::String::ToLowerR(v_search_str);

			switch (m_cb_selectedGenerator->SelectedIndex)
			{
			case Generator_BlueprintConverter:
				BlueprintFolderReader::Search(v_search_str, last_search_length, m_tb_searchBox->TextLength);
				break;
			case Generator_TileConverter:
				TileFolderReader::Search(v_search_str, last_search_length, m_tb_searchBox->TextLength);
				break;
			case Generator_WorldConverter:
				WorldFolderReader::Search(v_search_str, last_search_length, m_tb_searchBox->TextLength);
				break;
			case Generator_CharacterConverter:
				UserCharacterReader::Search(v_search_str, last_search_length, m_tb_searchBox->TextLength);
				break;
			}
		}

		this->UpdateCurrentObjectList();
	}

	void MainGui::MainGui_SearchBox_TextChanged(System::Object^ sender, System::EventArgs^ e)
	{
		this->UpdateSearchResults(m_lastSearchLength);
		this->UpdateConvertButton();
		this->UpdateContextMenuStrip();
		this->MainGui_UpdatePathTextBox();

		m_lastSearchLength = m_tb_searchBox->TextLength;
	}

	void MainGui::UpdateConvertButton()
	{
		m_btn_convert->Enabled = (m_tb_path->TextLength > 0 || m_lb_objectSelector->SelectedIndex >= 0) && m_database_isLoaded;
	}

	void MainGui::ResetProgressBar()
	{
		m_pb_progress->Value = 0;
		m_pb_progress->Maximum = 0;
	}

	void MainGui::MainGui_ConvertBlueprint(const std::wstring& filename, const std::wstring& path)
	{
		if (m_bw_objectConverter->IsBusy) return;

		BlueprintConvertSettings^ v_conv_settings = gcnew BlueprintConvertSettings(filename.c_str(), path.c_str());
		this->MainGui_CenterChildForm(v_conv_settings);
		v_conv_settings->ShowDialog();

		if (!v_conv_settings->m_ready_to_convert)
			return;

		System::Array^ v_thread_data = gcnew cli::array<System::Object^>(8);

		//Data type
		v_thread_data->SetValue(safe_cast<System::Object^>(static_cast<int>(Generator_BlueprintConverter)), static_cast<int>(0));

		//Tile path and name data
		v_thread_data->SetValue(gcnew System::String(path.c_str())  , static_cast<int>(1));
		v_thread_data->SetValue(v_conv_settings->m_tb_filename->Text, static_cast<int>(2));

		//Blueprint settings
		v_thread_data->SetValue(v_conv_settings->m_cb_separationType->SelectedIndex, static_cast<int>(3));

		//Model settings
		v_thread_data->SetValue(v_conv_settings->m_cb_exportMaterials->Checked, static_cast<int>(4));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportNormals->Checked  , static_cast<int>(5));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportUvs->Checked      , static_cast<int>(6));

		v_thread_data->SetValue(v_conv_settings->m_cb_customGame->SelectedIndex, static_cast<int>(7));

		this->MainGui_ChangeGuiState(m_database_isLoaded, m_obj_isLoaded, false);
		m_progressBarUpdater->Start();

		m_bw_objectConverter->RunWorkerAsync(v_thread_data);
	}

	void MainGui::MainGui_ConvertTile(const std::wstring& filename, const std::wstring& path)
	{
		if (m_bw_objectConverter->IsBusy) return;

		TileConvertSettings^ v_conv_settings = gcnew TileConvertSettings(filename.c_str(), path.c_str());
		this->MainGui_CenterChildForm(v_conv_settings);
		v_conv_settings->ShowDialog();

		if (!v_conv_settings->m_ready_to_convert)
			return;

		System::Array^ v_thread_data = gcnew cli::array<System::Object^>(16);

		//Data type
		v_thread_data->SetValue(static_cast<int>(Generator_TileConverter), static_cast<int>(0));

		//Tile path and name data
		v_thread_data->SetValue(gcnew System::String(path.c_str())  , static_cast<int>(1));
		v_thread_data->SetValue(v_conv_settings->m_tb_filename->Text, static_cast<int>(2));

		//Tile settings
		v_thread_data->SetValue(v_conv_settings->m_cb_exportHarvestables->Checked, static_cast<int>(3));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportBlueprints->Checked  , static_cast<int>(4));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportKinematics->Checked  , static_cast<int>(5));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportClutter->Checked     , static_cast<int>(6));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportPrefabs->Checked     , static_cast<int>(7));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportDecals->Checked      , static_cast<int>(8));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportAssets->Checked      , static_cast<int>(9));

		//Model settings
		v_thread_data->SetValue(v_conv_settings->m_cb_export8kGndTextures->Checked, static_cast<int>(10));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportGndTextures->Checked  , static_cast<int>(11));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportMaterials->Checked    , static_cast<int>(12));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportNormals->Checked      , static_cast<int>(13));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportUvs->Checked          , static_cast<int>(14));

		v_thread_data->SetValue(v_conv_settings->m_cb_customGame->SelectedIndex, static_cast<int>(15));

		this->MainGui_ChangeGuiState(m_database_isLoaded, m_obj_isLoaded, false);
		m_progressBarUpdater->Start();

		m_bw_objectConverter->RunWorkerAsync(v_thread_data);
	}

	void MainGui::MainGui_ConvertWorld(const std::wstring& filename, const std::wstring& path)
	{
		if (m_bw_objectConverter->IsBusy) return;

		TileConvertSettings^ v_conv_settings = gcnew TileConvertSettings(filename.c_str(), path.c_str());
		this->MainGui_CenterChildForm(v_conv_settings);
		v_conv_settings->ShowDialog();

		if (!v_conv_settings->m_ready_to_convert)
			return;

		System::Array^ v_thread_data = gcnew cli::array<System::Object^>(16);

		//Data type
		v_thread_data->SetValue(static_cast<int>(Generator_WorldConverter), static_cast<int>(0));

		//Tile path and name data
		v_thread_data->SetValue(gcnew System::String(path.c_str()), static_cast<int>(1));
		v_thread_data->SetValue(v_conv_settings->m_tb_filename->Text, static_cast<int>(2));

		//Tile settings
		v_thread_data->SetValue(v_conv_settings->m_cb_exportHarvestables->Checked, static_cast<int>(3));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportBlueprints->Checked, static_cast<int>(4));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportKinematics->Checked, static_cast<int>(5));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportClutter->Checked, static_cast<int>(6));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportPrefabs->Checked, static_cast<int>(7));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportDecals->Checked, static_cast<int>(8));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportAssets->Checked, static_cast<int>(9));

		//Model settings
		v_thread_data->SetValue(v_conv_settings->m_cb_export8kGndTextures->Checked, static_cast<int>(10));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportGndTextures->Checked, static_cast<int>(11));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportMaterials->Checked, static_cast<int>(12));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportNormals->Checked, static_cast<int>(13));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportUvs->Checked, static_cast<int>(14));

		v_thread_data->SetValue(v_conv_settings->m_cb_customGame->SelectedIndex, static_cast<int>(15));

		this->MainGui_ChangeGuiState(m_database_isLoaded, m_obj_isLoaded, false);
		m_progressBarUpdater->Start();

		m_bw_objectConverter->RunWorkerAsync(v_thread_data);
	}
	
	void MainGui::MainGui_ConvertCharacter(const std::wstring& filename, const UserCharacterData& v_data)
	{
		System::Array^ v_thread_data = gcnew cli::array<System::Object^>(2 + sizeof(UserCharacterData));

		v_thread_data->SetValue(static_cast<int>(Generator_CharacterConverter), static_cast<int>(0));
		v_thread_data->SetValue(gcnew System::String(filename.c_str()), static_cast<int>(1));

		//Convert character data to bytes and assemble it back on the worker thread
		for (std::size_t a = 0; a < sizeof(UserCharacterData); a++)
			v_thread_data->SetValue(reinterpret_cast<const unsigned char*>(&v_data)[a], static_cast<int>(2 + a));

		this->MainGui_ChangeGuiState(m_database_isLoaded, m_obj_isLoaded, false);
		m_progressBarUpdater->Start();

		m_bw_objectConverter->RunWorkerAsync(v_thread_data);
	}

	void MainGui::MainGui_Convert_Clicked(System::Object^ sender, System::EventArgs^ e)
	{
		if (m_tb_path->TextLength == 0 && m_lb_objectSelector->SelectedIndex == -1)
			return;

		if (m_lb_objectSelector->SelectedIndex >= 0)
		{
			switch (m_cb_selectedGenerator->SelectedIndex)
			{
			case Generator_BlueprintConverter:
				{
					BlueprintInstance* v_cur_bp = this->GetCurrentBlueprint();
					this->MainGui_ConvertBlueprint(v_cur_bp->name, v_cur_bp->path);
					break;
				}
			case Generator_TileConverter:
				{
					TileInstance* v_cur_tile = this->GetCurrentTile();
					this->MainGui_ConvertTile(v_cur_tile->name, v_cur_tile->path);
					break;
				}
			case Generator_WorldConverter:
				{
					WorldInstance* v_cur_world = this->GetCurrentWorld();
					this->MainGui_ConvertWorld(v_cur_world->name, v_cur_world->path);
					break;
				}
			case Generator_CharacterConverter:
				{
					UserCharacterInstance* v_cur_char = this->GetCurrentCharacter();
					this->MainGui_ConvertCharacter(v_cur_char->name, v_cur_char->character_data);
					break;
				}
			}
		}
		else
		{
			namespace fs = std::filesystem;

			const std::wstring v_path_wstr = msclr::interop::marshal_as<std::wstring>(m_tb_path->Text);
			if (!File::Exists(v_path_wstr))
			{
				WF_SHOW_WARNING("Invalid Path", "File doesn't exist!");
				return;
			}

			if (!File::IsRegularFile(v_path_wstr))
			{
				WF_SHOW_WARNING("Invalid Path", "The path must lead to a file");
				return;
			}

			const fs::path v_cur_path = v_path_wstr;

			switch (m_cb_selectedGenerator->SelectedIndex)
			{
			case Generator_BlueprintConverter:
				{
					const std::wstring v_bp_name = (v_cur_path.has_stem() ? v_cur_path.stem().wstring() : L"UnknownBlueprint");

					this->MainGui_ConvertBlueprint(v_bp_name, v_path_wstr);
					break;
				}
			case Generator_TileConverter:
				{
					const std::wstring v_tile_name = (v_cur_path.has_stem() ? v_cur_path.stem().wstring() : L"UnknownTile");

					this->MainGui_ConvertTile(v_tile_name, v_path_wstr);
					break;
				}
			case Generator_WorldConverter:
				{
					const std::wstring v_world_name = (v_cur_path.has_stem() ? v_cur_path.stem().wstring() : L"UnknownWorld");

					this->MainGui_ConvertWorld(v_world_name, v_path_wstr);
					break;
				}
			case Generator_CharacterConverter:
				//TODO: Implement that
				break;
			}
		}
	}

	std::vector<BlueprintInstance*>& MainGui::GetCurrentBlueprintList()
	{
		return (m_tb_searchBox->TextLength > 0) ? BlueprintFolderReader::SearchResults : BlueprintFolderReader::GetCurrentStorage();
	}

	BlueprintInstance* MainGui::GetCurrentBlueprint()
	{
		if (m_lb_objectSelector->SelectedIndex == -1)
			return nullptr;

		return this->GetCurrentBlueprintList()[m_lb_objectSelector->SelectedIndex];
	}

	std::vector<TileInstance*>& MainGui::GetCurrentTileList()
	{
		return (m_tb_searchBox->TextLength > 0) ? TileFolderReader::SearchResults : TileFolderReader::GetCurrentStorage();
	}

	TileInstance* MainGui::GetCurrentTile()
	{
		if (m_lb_objectSelector->SelectedIndex == -1)
			return nullptr;

		return this->GetCurrentTileList()[m_lb_objectSelector->SelectedIndex];
	}

	std::vector<WorldInstance*>& MainGui::GetCurrentWorldList()
	{
		return (m_tb_searchBox->TextLength > 0) ? WorldFolderReader::SearchResults : WorldFolderReader::GetCurrentStorage();
	}

	WorldInstance* MainGui::GetCurrentWorld()
	{
		if (m_lb_objectSelector->SelectedIndex == -1)
			return nullptr;

		return this->GetCurrentWorldList()[m_lb_objectSelector->SelectedIndex];
	}

	std::vector<UserCharacterInstance*>& MainGui::GetCurrentCharacterList()
	{
		return (m_tb_searchBox->TextLength > 0) ? UserCharacterReader::SearchResults : UserCharacterReader::GetCurrentStorage();
	}

	UserCharacterInstance* MainGui::GetCurrentCharacter()
	{
		if (m_lb_objectSelector->SelectedIndex == -1)
			return nullptr;

		return this->GetCurrentCharacterList()[m_lb_objectSelector->SelectedIndex];
	}

	void MainGui::MainGui_HandleConvertError(ConvertError& v_error, const int& v_type, System::ComponentModel::DoWorkEventArgs^ e)
	{
		System::Array^ v_op_result = nullptr;
		if (v_error)
		{
			v_op_result = gcnew cli::array<System::Object^>(3);

			v_op_result->SetValue(true, static_cast<int>(1));
			v_op_result->SetValue(gcnew System::String(v_error.getErrorMsg().c_str()), static_cast<int>(2));
		}
		else
		{
			v_op_result = gcnew cli::array<System::Object^>(2);

			v_op_result->SetValue(false, static_cast<int>(1));
		}

		v_op_result->SetValue(v_type, static_cast<int>(0));

		e->Result = v_op_result;
	}

	void MainGui::ObjectConverter_ConvertBlueprint(System::Array^ conv_data, System::ComponentModel::DoWorkEventArgs^ e)
	{
		DebugOutL(__FUNCTION__);

		System::Array^ v_conv_data = safe_cast<System::Array^>(e->Argument);

		const std::wstring v_bp_path = msclr::interop::marshal_as<std::wstring>(safe_cast<System::String^>(v_conv_data->GetValue(static_cast<int>(1))));

		std::wstring v_bp_name = msclr::interop::marshal_as<std::wstring>(safe_cast<System::String^>(v_conv_data->GetValue(static_cast<int>(2))));
		::String::RemoveSpacesFromEndR(v_bp_name);

		//Load blueprint settings
		BlueprintConverterSettings::SeparationType = safe_cast<int>(v_conv_data->GetValue(static_cast<int>(3)));

		//Load Model Settings
		SharedConverterSettings::ExportMaterials = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(4)));
		SharedConverterSettings::ExportNormals   = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(5)));
		SharedConverterSettings::ExportUvs       = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(6)));

		const std::size_t v_custom_game_idx = static_cast<std::size_t>(safe_cast<int>(v_conv_data->GetValue(static_cast<int>(7))));
		CustomGame* v_custom_game = nullptr;
		if (v_custom_game_idx > 0)
			v_custom_game = SMMod::GetCustomGames()[v_custom_game_idx - 1];

		//Error check some settings
		SharedConverterSettings::ExportMaterials &= SharedConverterSettings::ExportUvs;

		ConvertError v_conv_error;
		BlueprintConv::ConvertToModel(v_bp_path, v_bp_name, v_conv_error, v_custom_game);

		this->MainGui_HandleConvertError(v_conv_error, Generator_BlueprintConverter, e);
	}

	void MainGui::ObjectConverter_ConvertTile(System::Array^ conv_data, System::ComponentModel::DoWorkEventArgs^ e)
	{
		DebugOutL(__FUNCTION__);

		System::Array^ v_conv_data = safe_cast<System::Array^>(e->Argument);

		const std::wstring v_tile_path = msclr::interop::marshal_as<std::wstring>(safe_cast<System::String^>(v_conv_data->GetValue(static_cast<int>(1))));

		std::wstring v_tile_name = msclr::interop::marshal_as<std::wstring>(safe_cast<System::String^>(v_conv_data->GetValue(static_cast<int>(2))));
		::String::RemoveSpacesFromEndR(v_tile_name);

		//Load the tile settings
		TileConverterSettings::ExportHarvestables = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(3)));
		TileConverterSettings::ExportBlueprints   = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(4)));
		TileConverterSettings::ExportKinematics   = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(5)));
		TileConverterSettings::ExportClutter      = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(6)));
		TileConverterSettings::ExportPrefabs      = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(7)));
		TileConverterSettings::ExportDecals       = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(8)));
		TileConverterSettings::ExportAssets       = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(9)));

		//Load the model settings
		TileConverterSettings::Export8kGroundTextures = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(10)));
		TileConverterSettings::ExportGroundTextures   = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(11)));
		SharedConverterSettings::ExportMaterials      = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(12)));
		SharedConverterSettings::ExportNormals        = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(13)));
		SharedConverterSettings::ExportUvs            = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(14)));

		const std::size_t v_custom_game_idx = static_cast<std::size_t>(safe_cast<int>(v_conv_data->GetValue(static_cast<int>(15))));
		CustomGame* v_custom_game = nullptr;
		if (v_custom_game_idx > 0)
			v_custom_game = SMMod::GetCustomGames()[v_custom_game_idx - 1];

		//Error check some settings
		TileConverterSettings::Export8kGroundTextures &= TileConverterSettings::ExportGroundTextures;
		SharedConverterSettings::ExportMaterials &= SharedConverterSettings::ExportUvs;

		ConvertError v_conv_error;
		TileConv::ConvertToModel(v_tile_path, v_tile_name, v_conv_error, v_custom_game);

		this->MainGui_HandleConvertError(v_conv_error, Generator_TileConverter, e);
	}

	void MainGui::ObjectConverter_ConvertWorld(System::Array^ conv_data, System::ComponentModel::DoWorkEventArgs^ e)
	{
		DebugOutL(__FUNCTION__);

		System::Array^ v_conv_data = safe_cast<System::Array^>(e->Argument);

		const std::wstring v_world_path = msclr::interop::marshal_as<std::wstring>(safe_cast<System::String^>(v_conv_data->GetValue(static_cast<int>(1))));

		std::wstring v_world_name = msclr::interop::marshal_as<std::wstring>(safe_cast<System::String^>(v_conv_data->GetValue(static_cast<int>(2))));
		::String::RemoveSpacesFromEndR(v_world_name);

		//Load the tile settings
		TileConverterSettings::ExportHarvestables = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(3)));
		TileConverterSettings::ExportBlueprints = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(4)));
		TileConverterSettings::ExportKinematics = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(5)));
		TileConverterSettings::ExportClutter = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(6)));
		TileConverterSettings::ExportPrefabs = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(7)));
		TileConverterSettings::ExportDecals = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(8)));
		TileConverterSettings::ExportAssets = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(9)));

		//Load the model settings
		TileConverterSettings::Export8kGroundTextures = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(10)));
		TileConverterSettings::ExportGroundTextures = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(11)));
		SharedConverterSettings::ExportMaterials = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(12)));
		SharedConverterSettings::ExportNormals = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(13)));
		SharedConverterSettings::ExportUvs = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(14)));

		const std::size_t v_custom_game_idx = static_cast<std::size_t>(safe_cast<int>(v_conv_data->GetValue(static_cast<int>(15))));
		CustomGame* v_custom_game = nullptr;
		if (v_custom_game_idx > 0)
			v_custom_game = SMMod::GetCustomGames()[v_custom_game_idx - 1];

		//Error check some settings
		TileConverterSettings::Export8kGroundTextures &= TileConverterSettings::ExportGroundTextures;
		SharedConverterSettings::ExportMaterials &= SharedConverterSettings::ExportUvs;

		ConvertError v_conv_error;
		WorldConverter::ConvertToModel(v_world_path, v_world_name, v_conv_error, v_custom_game);

		this->MainGui_HandleConvertError(v_conv_error, Generator_TileConverter, e);
	}

	void MainGui::ObjectConverter_ConvertCharacter(System::Array^ conv_data, System::ComponentModel::DoWorkEventArgs^ e)
	{
		DebugOutL(__FUNCTION__);

		System::Array^ v_conv_data = safe_cast<System::Array^>(e->Argument);
		if (v_conv_data->LongLength != (sizeof(UserCharacterData) + 2))
		{
			System::Array^ v_op_result = gcnew cli::array<System::Object^>(3);
			v_op_result->SetValue(static_cast<int>(Generator_CharacterConverter), static_cast<int>(0));
			v_op_result->SetValue(true, static_cast<int>(1));
			v_op_result->SetValue("Worker thread received incorrect data!", static_cast<int>(2));

			e->Result = v_op_result;
			return;
		}

		const std::wstring v_filename = msclr::interop::marshal_as<std::wstring>(safe_cast<System::String^>(v_conv_data->GetValue(1ll)));

		//Assemble the character data back
		UserCharacterData v_char_data{};
		for (std::size_t a = 0; a < sizeof(UserCharacterData); a++)
			 reinterpret_cast<unsigned char*>(&v_char_data)[a] = safe_cast<unsigned char>(v_conv_data->GetValue(static_cast<long long>(a + 2)));

		ConvertError v_conv_error;
		CharacterConverter::ConvertToModel(v_char_data, v_filename, v_conv_error);

		this->MainGui_HandleConvertError(v_conv_error, Generator_CharacterConverter, e);
	}

	void MainGui::MainGui_ObjectConverter_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e)
	{
		System::Array^ v_thread_data = safe_cast<System::Array^>(e->Argument);

		const unsigned short v_generator_type = static_cast<unsigned short>(safe_cast<int>(v_thread_data->GetValue(static_cast<int>(0))));
		switch (v_generator_type)
		{
		case Generator_BlueprintConverter:
			this->ObjectConverter_ConvertBlueprint(v_thread_data, e);
			break;
		case Generator_TileConverter:
			this->ObjectConverter_ConvertTile(v_thread_data, e);
			break;
		case Generator_WorldConverter:
			this->ObjectConverter_ConvertWorld(v_thread_data, e);
			break;
		case Generator_CharacterConverter:
			this->ObjectConverter_ConvertCharacter(v_thread_data, e);
			break;
		}
	}

	void MainGui::MainGui_ObjectConverter_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e)
	{
		m_progressBarUpdater->Stop();
		this->MainGui_Timer_UpdateProgressBar(nullptr, nullptr);

		System::Array^ v_work_result = safe_cast<System::Array^>(e->Result);

		const unsigned short v_type_id = static_cast<unsigned short>(safe_cast<int>(v_work_result->GetValue(static_cast<int>(0))));
		const bool v_has_error = safe_cast<bool>(v_work_result->GetValue(static_cast<int>(1)));

		if (v_has_error)
		{
			System::String^ v_error_msg = safe_cast<System::String^>(v_work_result->GetValue(static_cast<int>(2)));
			ProgCounter::SetState(ProgState::ConvertFailure, 0);

			System::Windows::Forms::MessageBox::Show(
				v_error_msg,
				"Convert Error",
				System::Windows::Forms::MessageBoxButtons::OK,
				System::Windows::Forms::MessageBoxIcon::Error
			);
		}
		else
		{
			ProgCounter::SetState(ProgState::ConvertSuccess, 0);

			const static char* g_resultMessages[] =
			{
				"Blueprint has been successfully converted!",
				"Tile has been successfully converted!",
				"World has been successfully converted!",
				"Character has been successfully converted!"
			};

			System::Windows::Forms::MessageBox::Show(
				gcnew System::String(g_resultMessages[v_type_id]),
				"Convert Success",
				System::Windows::Forms::MessageBoxButtons::OK,
				System::Windows::Forms::MessageBoxIcon::Information
			);
		}

		//Update the status message
		this->ResetProgressBar();
		this->MainGui_Timer_UpdateProgressBar(nullptr, nullptr);

		this->MainGui_ChangeGuiState(m_database_isLoaded, m_obj_isLoaded, true);
	}

	void MainGui::MainGui_Options_Click(System::Object^ sender, System::EventArgs^ e)
	{
		SettingsGui^ v_settings_gui = gcnew SettingsGui();
		this->MainGui_CenterChildForm(v_settings_gui);
		v_settings_gui->ShowDialog();

		if (v_settings_gui->m_reload_obj_db)
		{
			DatabaseConfig::ReadConfig();
			GroundTextureDatabase::Initialize();

			this->LoadObjectDatabase(false);
		}

		if (v_settings_gui->m_reload_user_obj)
			this->LoadUserObjects();
	}

	void MainGui::MainGui_OpenItemInWorkshop(System::Object^ sender, System::EventArgs^ e)
	{
		if (m_lb_objectSelector->SelectedIndex == -1) return;

		unsigned long long v_workshop_id = 0;
		switch (m_cb_selectedGenerator->SelectedIndex)
		{
		case Generator_BlueprintConverter:
			v_workshop_id = this->GetCurrentBlueprint()->workshop_id;
			break;
		case Generator_TileConverter:
			v_workshop_id = this->GetCurrentTile()->workshop_id;
			break;
		case Generator_WorldConverter:
			v_workshop_id = this->GetCurrentWorld()->workshop_id;
			break;
		}

		if (v_workshop_id == 0)
		{
			WF_SHOW_WARNING("Can't open", "Invalid workshop id");
			return;
		}

		std::wstring v_workshop_url;
		if (DatabaseConfig::OpenLinksInSteam)
			v_workshop_url.append(L"steam://openurl/");

		v_workshop_url.append(L"https://steamcommunity.com/sharedfiles/filedetails/?id=");
		v_workshop_url.append(std::to_wstring(v_workshop_id));

		System::Diagnostics::Process::Start(gcnew System::String(v_workshop_url.c_str()));
	}

	void MainGui::MainGui_OpenItemDirectory(System::Object^ sender, System::EventArgs^ e)
	{
		if (m_lb_objectSelector->SelectedIndex == -1) return;

		std::wstring v_path_to_open;
		switch (m_cb_selectedGenerator->SelectedIndex)
		{
		case Generator_BlueprintConverter:
			v_path_to_open = this->GetCurrentBlueprint()->path;
			break;
		case Generator_TileConverter:
			v_path_to_open = this->GetCurrentTile()->path;
			break;
		case Generator_WorldConverter:
			v_path_to_open = this->GetCurrentWorld()->path;
			break;
		case Generator_CharacterConverter:
			//TODO: Implement that
			break;
		}

		::String::ReplaceAllR(v_path_to_open, L'/', L'\\');
		if (!File::Exists(v_path_to_open))
		{
			WF_SHOW_WARNING("Invalid item directory", "Path doesn't exist");
			return;
		}

		const std::wstring v_final_path = L"/select,\"" + v_path_to_open + L'\"';
		System::Diagnostics::Process::Start("explorer.exe", gcnew System::String(v_final_path.c_str()));
	}

	void MainGui::UpdateContextMenuStrip()
	{
		switch (m_cb_selectedGenerator->SelectedIndex)
		{
		case Generator_BlueprintConverter:
			{
				bool v_has_workshop_id = false;
				bool v_has_directory = false;

				BlueprintInstance* v_cur_bp = this->GetCurrentBlueprint();
				if (v_cur_bp)
				{
					v_has_workshop_id = v_cur_bp->workshop_id != 0;
					v_has_directory = File::Exists(v_cur_bp->path);
				}

				m_btn_openBlueprintFolder->Enabled = v_has_directory;
				m_btn_openBlueprintInSteamWorkshop->Enabled = v_has_workshop_id;
				m_btn_showBlueprintInfo->Enabled = (m_lb_objectSelector->SelectedIndex != -1);

				break;
			}
		case Generator_TileConverter:
			{
				bool v_has_workshop_id = false;
				bool v_has_creator_id = false;
				bool v_has_directory = false;

				TileInstance* v_cur_tile = this->GetCurrentTile();
				if (v_cur_tile)
				{
					v_has_workshop_id = v_cur_tile->workshop_id != 0;
					v_has_creator_id = v_cur_tile->creator_id != 0;
					v_has_directory = File::Exists(v_cur_tile->path);
				}

				m_btn_openTileFolder->Enabled = v_has_directory;
				m_btn_openTileInSteamWorkshop->Enabled = v_has_workshop_id;
				m_btn_findTileCreatorInSteam->Enabled = v_has_creator_id;
				m_btn_showTileInfo->Enabled = (m_lb_objectSelector->SelectedIndex != -1);

				break;
			}
		case Generator_WorldConverter:
			{
				bool v_has_workshop_id = false;
				bool v_has_directory = false;

				WorldInstance* v_cur_world = this->GetCurrentWorld();
				if (v_cur_world)
				{
					v_has_workshop_id = v_cur_world->workshop_id != 0;
					v_has_directory = File::Exists(v_cur_world->path);
				}

				m_btn_openWorldFolder->Enabled = v_has_directory;
				m_btn_openWorldInSteamWorkshop->Enabled = v_has_workshop_id;
				m_btn_showWorldInfo->Enabled = (m_lb_objectSelector->SelectedIndex != -1);

				break;
			}
		case Generator_CharacterConverter:
			{
				bool v_has_directory = false;

				UserCharacterInstance* v_cur_character = this->GetCurrentCharacter();
				if (v_cur_character)
				{
					v_has_directory = File::Exists(v_cur_character->path);
				}

				//TODO: Implement context menu strip

				break;
			}
		}
	}

	void MainGui::MainGui_FindTileCreator_Click(System::Object^ sender, System::EventArgs^ e)
	{
		if (m_lb_objectSelector->SelectedIndex == -1) return;
		if (m_cb_selectedGenerator->SelectedIndex != Generator_TileConverter) return;

		TileInstance* v_cur_tile = this->GetCurrentTile();
		if (v_cur_tile->creator_id == 0)
		{
			WF_SHOW_WARNING("Can't open", "Invalid user id");
			return;
		}

		std::wstring v_user_url;
		if (DatabaseConfig::OpenLinksInSteam)
			v_user_url.append(L"steam://openurl/");

		v_user_url.append(L"https://steamcommunity.com/profiles/");
		v_user_url.append(std::to_wstring(v_cur_tile->creator_id));

		System::Diagnostics::Process::Start(gcnew System::String(v_user_url.c_str()));
	}

	void MainGui::MainGui_OpenBlueprintOutputFolder_Click(System::Object^ sender, System::EventArgs^ e)
	{
		const std::wstring v_bp_out_dir = std::wstring(DatabaseConfig::BlueprintOutputFolder.data());
		if (File::CreateDirectorySafe(v_bp_out_dir))
			System::Diagnostics::Process::Start("explorer.exe", gcnew System::String(v_bp_out_dir.c_str()));
		else
			WF_SHOW_ERROR("Error", "Failed to create the blueprint output directory");
	}

	void MainGui::MainGui_OpenWorldOutputDirectory_Click(System::Object^ sender, System::EventArgs^ e)
	{
		const std::wstring v_world_out_dir = std::wstring(DatabaseConfig::WorldOutputFolder.data());
		if (File::CreateDirectorySafe(v_world_out_dir))
			System::Diagnostics::Process::Start("explorer.exe", gcnew System::String(v_world_out_dir.c_str()));
		else
			WF_SHOW_ERROR("Error", "Failed to create the world output directory");
	}

	void MainGui::MainGui_OpenTileOutputFolder_Click(System::Object^ sender, System::EventArgs^ e)
	{
		const std::wstring v_tile_out_dir = std::wstring(DatabaseConfig::TileOutputFolder.data());
		if (File::CreateDirectorySafe(v_tile_out_dir))
			System::Diagnostics::Process::Start("explorer.exe", gcnew System::String(v_tile_out_dir.c_str()));
		else
			WF_SHOW_ERROR("Error", "Failed to create the tile output directory!");
	}

	void MainGui::MainGui_Filter_Click(System::Object^ sender, System::EventArgs^ e)
	{
		FilterSettingsGui^ v_filter_settings = gcnew FilterSettingsGui();
		this->MainGui_CenterChildForm(v_filter_settings);
		v_filter_settings->ShowDialog();

		if (v_filter_settings->m_shouldReload)
			this->UpdateSearchResults(0);
	}

	void MainGui::MainGui_ShowBlueprintInfo_Click(System::Object^ sender, System::EventArgs^ e)
	{
		BlueprintInstance* v_cur_bp = this->GetCurrentBlueprint();
		if (!v_cur_bp) return;

		ItemInfoGui^ v_bp_info_gui = gcnew ItemInfoGui(v_cur_bp);
		this->MainGui_CenterChildForm(v_bp_info_gui);
		v_bp_info_gui->ShowDialog();
	}

	void MainGui::MainGui_ShowTileInfo_Click(System::Object^ sender, System::EventArgs^ e)
	{
		TileInstance* v_cur_tile = this->GetCurrentTile();
		if (!v_cur_tile) return;

		ItemInfoGui^ v_tile_info_gui = gcnew ItemInfoGui(v_cur_tile);
		if (v_tile_info_gui->m_isSuccess)
		{
			this->MainGui_CenterChildForm(v_tile_info_gui);
			v_tile_info_gui->ShowDialog();
		}
	}

	void MainGui::MainGui_ShowWorldInfo_Click(System::Object^ sender, System::EventArgs^ e)
	{
		WorldInstance* v_cur_world = this->GetCurrentWorld();
		if (!v_cur_world) return;

		ItemInfoGui^ v_world_info_gui = gcnew ItemInfoGui(v_cur_world);
		if (v_world_info_gui->m_isSuccess)
		{
			this->MainGui_CenterChildForm(v_world_info_gui);
			v_world_info_gui->ShowDialog();
		}
	}

	void MainGui::MainGui_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e)
	{
		namespace WForms = System::Windows::Forms;

		if (!m_bw_objectConverter->IsBusy) return;

		const WForms::DialogResult v_dialog_result = WForms::MessageBox::Show(
			"Do you really want to close the program while an item is being converted?\n\nClosing the program might produce a corrupt model!",
			"Closing",
			WForms::MessageBoxButtons::YesNo,
			WForms::MessageBoxIcon::Question
		);

		if (v_dialog_result != WForms::DialogResult::Yes)
			e->Cancel = true;
	}

	void MainGui::MainGui_CenterChildForm(System::Windows::Forms::Form^ form)
	{
		form->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
		form->Location = System::Drawing::Point(
			(this->Location.X + this->Size.Width / 2) - (form->Size.Width / 2),
			(this->Location.Y + this->Size.Height / 2) - (form->Size.Height / 2)
		);
	}
}
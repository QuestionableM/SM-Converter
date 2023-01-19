#include "MainGui.h"

#include "BlueprintConvertSettings.h"
#include "TileConvertSettings.h"
#include "SettingsGui.h"
#include "AboutGui.h"

#include "ObjectDatabase\GroundTextureDatabase.hpp"
#include "ObjectDatabase\KeywordReplacer.hpp"
#include "ObjectDatabase\ObjectDatabase.hpp"
#include "ObjectDatabase\DatabaseConfig.hpp"
#include "ObjectDatabase\ProgCounter.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Converter\BlueprintConverter\BlueprintConverter.hpp"
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

#define WF_SHOW_WARNING(title, message) \
	System::Windows::Forms::MessageBox::Show( \
		message, title, \
		System::Windows::Forms::MessageBoxButtons::OK, \
		System::Windows::Forms::MessageBoxIcon::Warning \
	)

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
				SettingsGui^ v_settings = gcnew SettingsGui();
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

	void MainGui::MainGui_UpdatePathListContextMenuStrip()
	{
		switch (m_cb_selectedGenerator->SelectedIndex)
		{
		case Generator_BlueprintConverter:
			m_lb_objectSelector->ContextMenuStrip = m_cms_blueprint;
			break;
		case Generator_TileConverter:
		case Generator_ScriptConverter:
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
			L"Search Scripts"
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
		this->ResetProgressBar();

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

		m_btn_convert->Enabled = db_loaded_and_obj_converted && (m_tb_path->TextLength > 0 || m_lb_objectSelector->SelectedIndex >= 0);
		m_btn_options->Enabled = db_loaded_and_obj_converted;

		m_cb_selectedGenerator->Enabled = obj_converted;
		m_menuStrip->Enabled = obj_converted;


		m_cms_blueprint->Enabled = objlist_and_obj_loaded;
		m_btn_openBlueprintFolder->Enabled = false;
		m_btn_openBlueprintInSteamWorkshop->Enabled = false;

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

	void MainGui::MainGui_ObjectLoader_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e)
	{
		//TODO: Make a script loader (not important for now)
		BlueprintFolderReader::ReadBlueprintsFromConfig();
		TileFolderReader::ReadTilesFromConfig();
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

	void MainGui::UpdateSearchResults(int last_search_length)
	{
		if (m_tb_searchBox->TextLength > 0)
		{
			std::wstring v_search_str = msclr::interop::marshal_as<std::wstring>(m_tb_searchBox->Text);
			::String::ToLowerR(v_search_str);

			switch (m_cb_selectedGenerator->SelectedIndex)
			{
			case Generator_BlueprintConverter:
				SearchFunction<BlueprintFolderReader>(v_search_str, last_search_length, m_tb_searchBox->TextLength);
				break;
			case Generator_TileConverter:
				SearchFunction<TileFolderReader>(v_search_str, last_search_length, m_tb_searchBox->TextLength);
				break;
			case Generator_ScriptConverter:
				break;
			}
		}

		this->UpdateCurrentObjectList();
	}

	void MainGui::MainGui_SearchBox_TextChanged(System::Object^ sender, System::EventArgs^ e)
	{
		this->UpdateSearchResults(m_lastSearchLength);
		this->UpdateConvertButton();
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

		BlueprintConvertSettings^ v_conv_settings = gcnew BlueprintConvertSettings(filename.c_str());
		v_conv_settings->ShowDialog();

		if (!v_conv_settings->m_ready_to_convert)
			return;

		System::Array^ v_thread_data = gcnew cli::array<System::Object^>(7);

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

		this->MainGui_ChangeGuiState(m_database_isLoaded, m_obj_isLoaded, false);
		m_progressBarUpdater->Start();

		m_bw_objectConverter->RunWorkerAsync(v_thread_data);
	}

	void MainGui::MainGui_ConvertTile(const std::wstring& filename, const std::wstring& path)
	{
		if (m_bw_objectConverter->IsBusy) return;

		TileConvertSettings^ v_conv_settings = gcnew TileConvertSettings(filename.c_str());
		v_conv_settings->ShowDialog();

		if (!v_conv_settings->m_ready_to_convert)
			return;

		System::Array^ v_thread_data = gcnew cli::array<System::Object^>(14);

		//Data type
		v_thread_data->SetValue(static_cast<int>(Generator_TileConverter), static_cast<int>(0));

		//Tile path and name data
		v_thread_data->SetValue(gcnew System::String(path.c_str())  , static_cast<int>(1));
		v_thread_data->SetValue(v_conv_settings->m_tb_filename->Text, static_cast<int>(2));

		//Tile settings
		v_thread_data->SetValue(v_conv_settings->m_cb_exportHarvestables->Checked, static_cast<int>(3));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportBlueprints->Checked  , static_cast<int>(4));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportClutter->Checked     , static_cast<int>(5));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportPrefabs->Checked     , static_cast<int>(6));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportDecals->Checked      , static_cast<int>(7));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportAssets->Checked      , static_cast<int>(8));

		//Model settings
		v_thread_data->SetValue(v_conv_settings->m_cb_export8kGndTextures->Checked, static_cast<int>(9));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportGndTextures->Checked  , static_cast<int>(10));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportMaterials->Checked    , static_cast<int>(11));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportNormals->Checked      , static_cast<int>(12));
		v_thread_data->SetValue(v_conv_settings->m_cb_exportUvs->Checked          , static_cast<int>(13));

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
					const std::vector<BlueprintInstance*>& v_bp_list = this->GetCurrentBlueprintList();
					BlueprintInstance* v_cur_bp = v_bp_list[m_lb_objectSelector->SelectedIndex];

					this->MainGui_ConvertBlueprint(v_cur_bp->name, v_cur_bp->path);
					break;
				}
			case Generator_TileConverter:
				{
					const std::vector<TileInstance*>& v_tile_list = this->GetCurrentTileList();
					TileInstance* v_cur_tile = v_tile_list[m_lb_objectSelector->SelectedIndex];

					this->MainGui_ConvertTile(v_cur_tile->name, v_cur_tile->path);
					break;
				}
			case Generator_ScriptConverter:
				break;
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

					this->MainGui_ConvertBlueprint(v_bp_name, v_cur_path);
					break;
				}
			case Generator_TileConverter:
				{
					const std::wstring v_tile_name = (v_cur_path.has_stem() ? v_cur_path.stem().wstring() : L"UnknownTile");

					this->MainGui_ConvertTile(v_tile_name, v_path_wstr);
					break;
				}
			case Generator_ScriptConverter:
				break;
			}
		}
	}

	std::vector<BlueprintInstance*>& MainGui::GetCurrentBlueprintList()
	{
		return (m_tb_searchBox->TextLength > 0) ? BlueprintFolderReader::SearchResults : BlueprintFolderReader::Storage;
	}

	BlueprintInstance* MainGui::GetCurrentBlueprint()
	{
		if (m_lb_objectSelector->SelectedIndex == -1)
			return nullptr;

		return this->GetCurrentBlueprintList()[m_lb_objectSelector->SelectedIndex];
	}

	std::vector<TileInstance*>& MainGui::GetCurrentTileList()
	{
		return (m_tb_searchBox->TextLength > 0) ? TileFolderReader::SearchResults : TileFolderReader::Storage;
	}

	TileInstance* MainGui::GetCurrentTile()
	{
		if (m_lb_objectSelector->SelectedIndex == -1)
			return nullptr;

		return this->GetCurrentTileList()[m_lb_objectSelector->SelectedIndex];
	}

	void MainGui::MainGui_HandleConvertError(ConvertError& v_error, const int& v_type, System::ComponentModel::DoWorkEventArgs^ e)
	{
		System::Array^ v_op_result = nullptr;
		if (v_error)
		{
			v_op_result = gcnew cli::array<System::Object^>(3);

			v_op_result->SetValue(true, static_cast<int>(1));

			const std::wstring& v_error_msg = v_error.GetErrorMsg();
			v_op_result->SetValue(gcnew System::String(v_error_msg.c_str()), static_cast<int>(2));
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
		const std::wstring v_bp_name = msclr::interop::marshal_as<std::wstring>(safe_cast<System::String^>(v_conv_data->GetValue(static_cast<int>(2))));

		//Load blueprint settings
		BlueprintConverterSettings::SeparationType = safe_cast<int>(v_conv_data->GetValue(static_cast<int>(3)));

		//Load Model Settings
		SharedConverterSettings::ExportMaterials = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(4)));
		SharedConverterSettings::ExportNormals   = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(5)));
		SharedConverterSettings::ExportUvs       = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(6)));

		//Error check some settings
		SharedConverterSettings::ExportMaterials &= SharedConverterSettings::ExportUvs;

		ConvertError v_conv_error;
		BlueprintConv::ConvertToModel(v_bp_path, v_bp_name, v_conv_error);

		this->MainGui_HandleConvertError(v_conv_error, Generator_BlueprintConverter, e);
	}

	void MainGui::ObjectConverter_ConvertTile(System::Array^ conv_data, System::ComponentModel::DoWorkEventArgs^ e)
	{
		DebugOutL(__FUNCTION__);

		System::Array^ v_conv_data = safe_cast<System::Array^>(e->Argument);

		const std::wstring v_tile_path = msclr::interop::marshal_as<std::wstring>(safe_cast<System::String^>(v_conv_data->GetValue(static_cast<int>(1))));
		const std::wstring v_tile_name = msclr::interop::marshal_as<std::wstring>(safe_cast<System::String^>(v_conv_data->GetValue(static_cast<int>(2))));

		//Load the tile settings
		TileConverterSettings::ExportHarvestables = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(3)));
		TileConverterSettings::ExportBlueprints   = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(4)));
		TileConverterSettings::ExportClutter      = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(5)));
		TileConverterSettings::ExportPrefabs      = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(6)));
		TileConverterSettings::ExportDecals       = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(7)));
		TileConverterSettings::ExportAssets       = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(8)));

		//Load the model settings
		TileConverterSettings::Export8kGroundTextures = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(9)));
		TileConverterSettings::ExportGroundTextures   = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(10)));
		SharedConverterSettings::ExportMaterials      = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(11)));
		SharedConverterSettings::ExportNormals        = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(12)));
		SharedConverterSettings::ExportUvs            = safe_cast<bool>(v_conv_data->GetValue(static_cast<int>(13)));

		//Error check some settings
		TileConverterSettings::Export8kGroundTextures &= TileConverterSettings::ExportGroundTextures;
		SharedConverterSettings::ExportMaterials &= SharedConverterSettings::ExportUvs;

		ConvertError v_conv_error;
		TileConv::ConvertToModel(v_tile_path, v_tile_name, v_conv_error);

		this->MainGui_HandleConvertError(v_conv_error, Generator_TileConverter, e);
	}

	void MainGui::ObjectConverter_ConvertScript(System::Array^ conv_data, System::ComponentModel::DoWorkEventArgs^ e)
	{
		DebugOutL(__FUNCTION__);

		System::Array^ v_op_result = gcnew cli::array<System::Object^>(2);

		v_op_result->SetValue(static_cast<int>(Generator_ScriptConverter), static_cast<int>(0));
		v_op_result->SetValue(false, static_cast<int>(1));

		e->Result = v_op_result;
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
		case Generator_ScriptConverter:
			this->ObjectConverter_ConvertScript(v_thread_data, e);
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
				"Script has been successfully converted!"
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
			v_path_to_open = this->GetCurrentBlueprint()->directory;
			break;
		case Generator_TileConverter:
			v_path_to_open = this->GetCurrentTile()->directory;
			break;
		case Generator_ScriptConverter:
			break;
		}

		::String::ReplaceAllR(v_path_to_open, L'/', L'\\');
		if (!File::Exists(v_path_to_open))
		{
			WF_SHOW_WARNING("Invalid item directory", "Path doesn't exist");
			return;
		}

		System::Diagnostics::Process::Start("explorer.exe", gcnew System::String(v_path_to_open.c_str()));
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
					v_has_directory = File::Exists(v_cur_bp->directory);
				}

				m_btn_openBlueprintFolder->Enabled = v_has_directory;
				m_btn_openBlueprintInSteamWorkshop->Enabled = v_has_workshop_id;

				break;
			}
		case Generator_TileConverter:
			break;
		case Generator_ScriptConverter:
			break;
		}
	}
}
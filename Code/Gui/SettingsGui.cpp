#include "SettingsGui.h"

#include "ObjectDatabase\DatabaseConfig.hpp"

#include "Utils\String.hpp"
#include "Utils\File.hpp"

#include <msclr\marshal_cppstd.h>

#define WF_SETTINGS_WARNING(title, message) \
	System::Windows::Forms::MessageBox::Show( \
		message, title, \
		System::Windows::Forms::MessageBoxButtons::OK, \
		System::Windows::Forms::MessageBoxIcon::Warning \
	)

enum : unsigned int
{
	SMFileOption_LocalModFolders    = 0,
	SMFileOption_WorkshopModFolders = 1,
	SMFileOption_BlueprintFolders   = 2,
	SMFileOption_TileFolders        = 3
};

namespace SMConverter
{
	SettingsGui::SettingsGui()
	{
		this->InitializeComponent();

		m_changeDetector = new SettingsChangeDetector();

		if (!m_changeDetector->m_gamePath.empty())
			m_tb_gamePath->Text = gcnew System::String(m_changeDetector->m_gamePath.c_str());

		m_cb_openLinksInSteam->Checked = DatabaseConfig::OpenLinksInSteam;

		m_cb_fileOption->SelectedIndex = 0;
	}

	SettingsGui::~SettingsGui()
	{
		delete m_changeDetector;

		if (components) delete components;
	}

	void SettingsGui::Settings_PathSelector_Click(System::Object^ sender, System::EventArgs^ e)
	{
		const std::wstring v_file_path = File::OpenFileDialog(
			L"Select a folder",
			FOS_FILEMUSTEXIST | FOS_PICKFOLDERS,
			L"All Files (*.*)\0*.*\0",
			static_cast<HWND>(this->Handle.ToPointer())
		);

		if (v_file_path.empty())
			return;

		m_tb_gamePath->Text = gcnew System::String(v_file_path.c_str());
	}

	void SettingsGui::Settings_FileOption_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
	{
		const static wchar_t* v_filePathMessages[] =
		{
			L"Enter a path to a local mod folder",
			L"Enter a path to a normal mod folder",
			L"Enter a path to a blueprint folder",
			L"Enter a path to a tile folder"
		};

		const wchar_t* v_current_message = v_filePathMessages[m_cb_fileOption->SelectedIndex];

		SendMessage(
			static_cast<HWND>(m_tb_filePath->Handle.ToPointer()),
			EM_SETCUEBANNER,
			0,
			reinterpret_cast<LPARAM>(v_current_message)
		);

		this->UpdateCurrentPathList();
	}

	void SettingsGui::UpdatePathListFromMap(const std::unordered_map<std::wstring, unsigned char>& v_map)
	{
		m_lb_pathList->BeginUpdate();
		m_lb_pathList->Items->Clear();

		for (const auto& v_cur_item : v_map)
			m_lb_pathList->Items->Add(gcnew System::String(v_cur_item.first.c_str()));

		m_lb_pathList->EndUpdate();
	}

	void SettingsGui::UpdatePathListFromVec(const std::vector<std::wstring>& v_path_list)
	{
		m_lb_pathList->BeginUpdate();
		m_lb_pathList->Items->Clear();

		for (const std::wstring& v_cur_wstr : v_path_list)
			m_lb_pathList->Items->Add(gcnew System::String(v_cur_wstr.c_str()));

		m_lb_pathList->EndUpdate();
	}

	void SettingsGui::UpdateCurrentPathList()
	{
		m_lb_pathList->SelectedIndex = -1;

		switch (m_cb_fileOption->SelectedIndex)
		{
		case SMFileOption_LocalModFolders:
			this->UpdatePathListFromVec(m_changeDetector->m_localModList);
			break;
		case SMFileOption_WorkshopModFolders:
			this->UpdatePathListFromVec(m_changeDetector->m_workshopModList);
			break;
		case SMFileOption_BlueprintFolders:
			this->UpdatePathListFromMap(m_changeDetector->m_blueprintFolders);
			break;
		case SMFileOption_TileFolders:
			this->UpdatePathListFromMap(m_changeDetector->m_tileFolders);
			break;
		}
	}

	void SettingsGui::Settings_PathList_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		if (e->Button != System::Windows::Forms::MouseButtons::Right) return;

		const int v_idx = m_lb_pathList->IndexFromPoint(e->X, e->Y);
		if (v_idx == -1) return;

		m_lb_pathList->SetSelected(v_idx, true);
	}

	void SettingsGui::Settings_PathList_KeyDown(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e)
	{
		if (e->KeyCode == Keys::Delete && m_lb_pathList->SelectedIndex >= 0)
		{
			this->Settings_RemovePath_Click(nullptr, nullptr);
			e->Handled = true;
		}
	}

	void SettingsGui::Settings_PathList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
	{
		const bool v_is_valid = (m_lb_pathList->SelectedIndex >= 0);
		m_btn_removePath->Enabled = v_is_valid;
		m_btn_openPathInExplorer->Enabled = v_is_valid;
	}

	void SettingsGui::Settings_FilePath_KeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e)
	{
		if (e->KeyChar == 13 && m_tb_filePath->TextLength > 0) //Check for Enter
		{
			this->Settings_AddPath_Click(nullptr, nullptr);
			e->Handled = true;
		}
	}

	void SettingsGui::Settings_FilePath_TextChanged(System::Object^ sender, System::EventArgs^ e)
	{
		m_btn_addPath->Enabled = (m_tb_filePath->TextLength > 0);
	}

	std::wstring SettingsGui::GetSelectedPathListString()
	{
		System::String^ v_str = m_lb_pathList->GetItemText(m_lb_pathList->SelectedItem);

		return msclr::interop::marshal_as<std::wstring>(v_str);
	}

	void SettingsGui::Settings_RemovePath_Click(System::Object^ sender, System::EventArgs^ e)
	{
		const int v_cur_idx = m_lb_pathList->SelectedIndex;
		if (v_cur_idx == -1) return;

		switch (m_cb_fileOption->SelectedIndex)
		{
		case SMFileOption_LocalModFolders:
			{
				SettingsChangeDetector::RemoveFromCheckedVec(m_changeDetector->m_localModList, m_changeDetector->m_modListMap, static_cast<std::size_t>(v_cur_idx));
				m_changeDetector->UpdateChange<SettingsChangeDetector_LocalModList>();
				break;
			}
		case SMFileOption_WorkshopModFolders:
			{
				SettingsChangeDetector::RemoveFromCheckedVec(m_changeDetector->m_workshopModList, m_changeDetector->m_modListMap, static_cast<std::size_t>(v_cur_idx));
				m_changeDetector->UpdateChange<SettingsChangeDetector_WorkshopModList>();
				break;
			}
		case SMFileOption_BlueprintFolders:
			{
				SettingsChangeDetector::RemoveFromMap(m_changeDetector->m_blueprintFolders, this->GetSelectedPathListString());
				m_changeDetector->UpdateChange<SettingsChangeDetector_BlueprintFolders>();
				break;
			}
		case SMFileOption_TileFolders:
			{
				SettingsChangeDetector::RemoveFromMap(m_changeDetector->m_tileFolders, this->GetSelectedPathListString());
				m_changeDetector->UpdateChange<SettingsChangeDetector_TileFolders>();
				break;
			}
		}

		this->UpdateCurrentPathList();
		this->UpdateSaveButton();

		m_lb_pathList->SelectedIndex = (v_cur_idx < m_lb_pathList->Items->Count) ? v_cur_idx : v_cur_idx - 1;
	}

	void SettingsGui::Settings_AddPath_Click(System::Object^ sender, System::EventArgs^ e)
	{
		if (m_tb_filePath->TextLength == 0) return;

		const std::wstring v_new_path = msclr::interop::marshal_as<std::wstring>(m_tb_filePath->Text);
		if (!File::Exists(v_new_path))
		{
			WF_SETTINGS_WARNING("Invalid Path", "The specified path doesn't exist!");
			return;
		}

		if (!File::IsDirectory(v_new_path))
		{
			WF_SETTINGS_WARNING("Invalid Path", "The specified path must lead to a directory");
			return;
		}

		bool is_success = false;

		switch (m_cb_fileOption->SelectedIndex)
		{
		case SMFileOption_LocalModFolders:
			{
				is_success = DatabaseConfig::AddToStrVec(m_changeDetector->m_localModList, m_changeDetector->m_modListMap, v_new_path);
				m_changeDetector->UpdateChange<SettingsChangeDetector_LocalModList>();
				break;
			}
		case SMFileOption_WorkshopModFolders:
			{
				is_success = DatabaseConfig::AddToStrVec(m_changeDetector->m_workshopModList, m_changeDetector->m_modListMap, v_new_path);
				m_changeDetector->UpdateChange<SettingsChangeDetector_WorkshopModList>();
				break;
			}
		case SMFileOption_BlueprintFolders:
			{
				is_success = DatabaseConfig::AddToStrMap(m_changeDetector->m_blueprintFolders, v_new_path);
				m_changeDetector->UpdateChange<SettingsChangeDetector_BlueprintFolders>();
				break;
			}
		case SMFileOption_TileFolders:
			{
				is_success = DatabaseConfig::AddToStrMap(m_changeDetector->m_tileFolders, v_new_path);
				m_changeDetector->UpdateChange<SettingsChangeDetector_TileFolders>();
				break;
			}
		}

		if (!is_success)
		{
			WF_SETTINGS_WARNING("Invalid Path", "The specified path already exists in the list!");
			return;
		}

		m_tb_filePath->Clear();
		this->UpdateCurrentPathList();
		this->UpdateSaveButton();
	}

	void SettingsGui::Settings_PathBrowser_Click(System::Object^ sender, System::EventArgs^ e)
	{
		const static wchar_t* v_openFileDialogTitles[] =
		{
			L"Select a local mod folder",
			L"Select a mod folder",
			L"Select a blueprint folder",
			L"Select a tile folder"
		};

		const std::wstring v_file_path = File::OpenFileDialog(
			v_openFileDialogTitles[m_cb_fileOption->SelectedIndex],
			FOS_FILEMUSTEXIST | FOS_PICKFOLDERS,
			L"All Files (*.*)\0*.*\0",
			static_cast<HWND>(this->Handle.ToPointer())
		);

		if (v_file_path.empty())
			return;

		m_tb_filePath->Text = gcnew System::String(v_file_path.c_str());
		m_tb_filePath->Focus();
	}

	void SettingsGui::Settings_SaveChanges_Click(System::Object^ sender, System::EventArgs^ e)
	{
		if (m_tb_gamePath->TextLength == 0)
		{
			WF_SETTINGS_WARNING("Empty Path", "The path to the game must be filled in");
			return;
		}

		const std::wstring v_game_path = msclr::interop::marshal_as<std::wstring>(m_tb_gamePath->Text);
		if (!File::Exists(v_game_path))
		{
			WF_SETTINGS_WARNING("Invalid Game Path", "The specified path to the game doesn't exist");
			return;
		}

		if (!File::IsDirectory(v_game_path))
		{
			WF_SETTINGS_WARNING("Invalid Game Path", "The path must lead to the root directory of the game");
			return;
		}

		m_reload_obj_db |= m_changeDetector->IsAnyBitSet(SettingsChangeDetector_GamePath);
		m_reload_user_obj |= m_changeDetector->IsAnyBitSet(
			SettingsChangeDetector_LocalModList | SettingsChangeDetector_WorkshopModList |
			SettingsChangeDetector_BlueprintFolders | SettingsChangeDetector_TileFolders
		);

		m_changeDetector->ApplyChanges();
		DatabaseConfig::SaveConfig();

		m_btn_saveChanges->Enabled = false;
	}

	void SettingsGui::Settings_OpenLinksInSteam_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		m_changeDetector->m_openLinksInSteam = m_cb_openLinksInSteam->Checked;
		m_changeDetector->UpdateChange<SettingsChangeDetector_OpenLinksInSteam>();

		this->UpdateSaveButton();
	}

	void SettingsGui::Settings_GamePath_TextChanged(System::Object^ sender, System::EventArgs^ e)
	{
		m_changeDetector->m_gamePath = msclr::interop::marshal_as<std::wstring>(m_tb_gamePath->Text);
		m_changeDetector->UpdateChange<SettingsChangeDetector_GamePath>();

		this->UpdateSaveButton();
	}

	void SettingsGui::UpdateSaveButton()
	{
		m_btn_saveChanges->Enabled = m_changeDetector->HasAnyChanges();
	}

	void SettingsGui::Settings_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e)
	{
		namespace WForms = System::Windows::Forms;

		if (!m_btn_saveChanges->Enabled)
			return;

		const WForms::DialogResult v_dialog_res = WForms::MessageBox::Show(
			"Do you want to leave the settings without saving?\n\nAll unsaved changes will be lost!",
			"Unsaved Changes",
			WForms::MessageBoxButtons::YesNo,
			WForms::MessageBoxIcon::Warning
		);

		if (v_dialog_res != WForms::DialogResult::Yes)
			e->Cancel = true;
	}

	void SettingsGui::Settings_OpenPathInExplorer_Click(System::Object^ sender, System::EventArgs^ e)
	{
		if (m_lb_pathList->SelectedIndex == -1) return;

		std::wstring v_selected_path = msclr::interop::marshal_as<std::wstring>(m_lb_pathList->GetItemText(m_lb_pathList->SelectedItem));
		::String::ReplaceAllR(v_selected_path, L'/', L'\\');

		if (!File::Exists(v_selected_path))
		{
			WF_SETTINGS_WARNING("Invalid Path", "Selected path is invalid");
			return;
		}

		System::Diagnostics::Process::Start("explorer.exe", gcnew System::String(v_selected_path.c_str()));
	}
}
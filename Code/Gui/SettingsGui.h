#pragma once

#include "SettingsGuiChangeDetector.hpp"

#include <unordered_map>
#include <string>
#include <vector>

namespace SMConverter
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class SettingsGui : public System::Windows::Forms::Form
	{
	public:
		SettingsGui(void);

	protected:
		~SettingsGui();

	private:
		SettingsChangeDetector* m_changeDetector = nullptr;

		System::Windows::Forms::Button^ m_btn_gamePathSelector;
		System::Windows::Forms::Button^ m_btn_saveChanges;
		System::Windows::Forms::Button^ m_btn_removePath;
		System::Windows::Forms::Button^ m_btn_browsePath;
		System::Windows::Forms::Button^ m_btn_addPath;

		System::Windows::Forms::CheckBox^ m_cb_openLinksInSteam;
		System::Windows::Forms::ComboBox^ m_cb_fileOption;
		System::Windows::Forms::TextBox^ m_tb_filePath;
		System::Windows::Forms::TextBox^ m_tb_gamePath;
		System::Windows::Forms::ListBox^ m_lb_pathList;

		System::Windows::Forms::TabControl^ m_tabControl;
		System::Windows::Forms::TabPage^ m_tabPage_general;
		System::Windows::Forms::TabPage^ m_tabPage_paths;

		System::Windows::Forms::Label^ m_lbl_gamePath;
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->m_tabControl = (gcnew System::Windows::Forms::TabControl());
			this->m_tabPage_general = (gcnew System::Windows::Forms::TabPage());
			this->m_btn_gamePathSelector = (gcnew System::Windows::Forms::Button());
			this->m_lbl_gamePath = (gcnew System::Windows::Forms::Label());
			this->m_cb_openLinksInSteam = (gcnew System::Windows::Forms::CheckBox());
			this->m_tb_gamePath = (gcnew System::Windows::Forms::TextBox());
			this->m_tabPage_paths = (gcnew System::Windows::Forms::TabPage());
			this->m_lb_pathList = (gcnew System::Windows::Forms::ListBox());
			this->m_cb_fileOption = (gcnew System::Windows::Forms::ComboBox());
			this->m_btn_removePath = (gcnew System::Windows::Forms::Button());
			this->m_btn_addPath = (gcnew System::Windows::Forms::Button());
			this->m_btn_browsePath = (gcnew System::Windows::Forms::Button());
			this->m_tb_filePath = (gcnew System::Windows::Forms::TextBox());
			this->m_btn_saveChanges = (gcnew System::Windows::Forms::Button());
			this->m_tabControl->SuspendLayout();
			this->m_tabPage_general->SuspendLayout();
			this->m_tabPage_paths->SuspendLayout();
			this->SuspendLayout();
			// 
			// m_tabControl
			// 
			this->m_tabControl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_tabControl->Controls->Add(this->m_tabPage_general);
			this->m_tabControl->Controls->Add(this->m_tabPage_paths);
			this->m_tabControl->Location = System::Drawing::Point(12, 12);
			this->m_tabControl->Name = L"m_tabControl";
			this->m_tabControl->SelectedIndex = 0;
			this->m_tabControl->Size = System::Drawing::Size(460, 296);
			this->m_tabControl->TabIndex = 0;
			// 
			// m_tabPage_general
			// 
			this->m_tabPage_general->Controls->Add(this->m_btn_gamePathSelector);
			this->m_tabPage_general->Controls->Add(this->m_lbl_gamePath);
			this->m_tabPage_general->Controls->Add(this->m_cb_openLinksInSteam);
			this->m_tabPage_general->Controls->Add(this->m_tb_gamePath);
			this->m_tabPage_general->Location = System::Drawing::Point(4, 22);
			this->m_tabPage_general->Name = L"m_tabPage_general";
			this->m_tabPage_general->Padding = System::Windows::Forms::Padding(3);
			this->m_tabPage_general->Size = System::Drawing::Size(452, 270);
			this->m_tabPage_general->TabIndex = 0;
			this->m_tabPage_general->Text = L"General";
			this->m_tabPage_general->UseVisualStyleBackColor = true;
			// 
			// m_btn_gamePathSelector
			// 
			this->m_btn_gamePathSelector->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->m_btn_gamePathSelector->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_btn_gamePathSelector->Location = System::Drawing::Point(406, 22);
			this->m_btn_gamePathSelector->Name = L"m_btn_gamePathSelector";
			this->m_btn_gamePathSelector->Size = System::Drawing::Size(40, 22);
			this->m_btn_gamePathSelector->TabIndex = 3;
			this->m_btn_gamePathSelector->Text = L"...";
			this->m_btn_gamePathSelector->UseVisualStyleBackColor = true;
			this->m_btn_gamePathSelector->Click += gcnew System::EventHandler(this, &SettingsGui::Settings_PathSelector_Click);
			// 
			// m_lbl_gamePath
			// 
			this->m_lbl_gamePath->AutoSize = true;
			this->m_lbl_gamePath->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lbl_gamePath->Location = System::Drawing::Point(3, 3);
			this->m_lbl_gamePath->Name = L"m_lbl_gamePath";
			this->m_lbl_gamePath->Size = System::Drawing::Size(188, 16);
			this->m_lbl_gamePath->TabIndex = 2;
			this->m_lbl_gamePath->Text = L"Path to Scrap Mechanic folder:";
			// 
			// m_cb_openLinksInSteam
			// 
			this->m_cb_openLinksInSteam->AutoSize = true;
			this->m_cb_openLinksInSteam->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_openLinksInSteam->Location = System::Drawing::Point(6, 50);
			this->m_cb_openLinksInSteam->Name = L"m_cb_openLinksInSteam";
			this->m_cb_openLinksInSteam->Size = System::Drawing::Size(148, 20);
			this->m_cb_openLinksInSteam->TabIndex = 1;
			this->m_cb_openLinksInSteam->Text = L"Open Links in Steam";
			this->m_cb_openLinksInSteam->UseVisualStyleBackColor = true;
			// 
			// m_tb_gamePath
			// 
			this->m_tb_gamePath->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_tb_gamePath->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_tb_gamePath->Location = System::Drawing::Point(6, 22);
			this->m_tb_gamePath->Name = L"m_tb_gamePath";
			this->m_tb_gamePath->Size = System::Drawing::Size(394, 22);
			this->m_tb_gamePath->TabIndex = 0;
			// 
			// m_tabPage_paths
			// 
			this->m_tabPage_paths->Controls->Add(this->m_lb_pathList);
			this->m_tabPage_paths->Controls->Add(this->m_cb_fileOption);
			this->m_tabPage_paths->Controls->Add(this->m_btn_removePath);
			this->m_tabPage_paths->Controls->Add(this->m_btn_addPath);
			this->m_tabPage_paths->Controls->Add(this->m_btn_browsePath);
			this->m_tabPage_paths->Controls->Add(this->m_tb_filePath);
			this->m_tabPage_paths->Location = System::Drawing::Point(4, 22);
			this->m_tabPage_paths->Name = L"m_tabPage_paths";
			this->m_tabPage_paths->Padding = System::Windows::Forms::Padding(3);
			this->m_tabPage_paths->Size = System::Drawing::Size(452, 270);
			this->m_tabPage_paths->TabIndex = 1;
			this->m_tabPage_paths->Text = L"Paths";
			this->m_tabPage_paths->UseVisualStyleBackColor = true;
			// 
			// m_lb_pathList
			// 
			this->m_lb_pathList->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_lb_pathList->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lb_pathList->FormattingEnabled = true;
			this->m_lb_pathList->IntegralHeight = false;
			this->m_lb_pathList->ItemHeight = 16;
			this->m_lb_pathList->Location = System::Drawing::Point(6, 34);
			this->m_lb_pathList->Name = L"m_lb_pathList";
			this->m_lb_pathList->Size = System::Drawing::Size(359, 200);
			this->m_lb_pathList->TabIndex = 5;
			this->m_lb_pathList->SelectedIndexChanged += gcnew System::EventHandler(this, &SettingsGui::Settings_PathList_SelectedIndexChanged);
			// 
			// m_cb_fileOption
			// 
			this->m_cb_fileOption->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_cb_fileOption->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->m_cb_fileOption->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_fileOption->FormattingEnabled = true;
			this->m_cb_fileOption->Items->AddRange(gcnew cli::array< System::Object^  >(4) {
				L"Local Mod Folders", L"Workshop Mod Folders",
					L"Blueprint Folders", L"Tile Folders"
			});
			this->m_cb_fileOption->Location = System::Drawing::Point(6, 240);
			this->m_cb_fileOption->Name = L"m_cb_fileOption";
			this->m_cb_fileOption->Size = System::Drawing::Size(440, 24);
			this->m_cb_fileOption->TabIndex = 4;
			this->m_cb_fileOption->SelectedIndexChanged += gcnew System::EventHandler(this, &SettingsGui::Settings_FileOption_SelectedIndexChanged);
			// 
			// m_btn_removePath
			// 
			this->m_btn_removePath->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->m_btn_removePath->Enabled = false;
			this->m_btn_removePath->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_btn_removePath->Location = System::Drawing::Point(371, 62);
			this->m_btn_removePath->Name = L"m_btn_removePath";
			this->m_btn_removePath->Size = System::Drawing::Size(75, 22);
			this->m_btn_removePath->TabIndex = 3;
			this->m_btn_removePath->Text = L"Remove";
			this->m_btn_removePath->UseVisualStyleBackColor = true;
			this->m_btn_removePath->Click += gcnew System::EventHandler(this, &SettingsGui::Settings_RemovePath_Click);
			// 
			// m_btn_addPath
			// 
			this->m_btn_addPath->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->m_btn_addPath->Enabled = false;
			this->m_btn_addPath->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_btn_addPath->Location = System::Drawing::Point(371, 34);
			this->m_btn_addPath->Name = L"m_btn_addPath";
			this->m_btn_addPath->Size = System::Drawing::Size(75, 22);
			this->m_btn_addPath->TabIndex = 2;
			this->m_btn_addPath->Text = L"Add";
			this->m_btn_addPath->UseVisualStyleBackColor = true;
			this->m_btn_addPath->Click += gcnew System::EventHandler(this, &SettingsGui::Settings_AddPath_Click);
			// 
			// m_btn_browsePath
			// 
			this->m_btn_browsePath->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->m_btn_browsePath->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_btn_browsePath->Location = System::Drawing::Point(371, 6);
			this->m_btn_browsePath->Name = L"m_btn_browsePath";
			this->m_btn_browsePath->Size = System::Drawing::Size(75, 22);
			this->m_btn_browsePath->TabIndex = 1;
			this->m_btn_browsePath->Text = L"...";
			this->m_btn_browsePath->UseVisualStyleBackColor = true;
			this->m_btn_browsePath->Click += gcnew System::EventHandler(this, &SettingsGui::Settings_PathBrowser_Click);
			// 
			// m_tb_filePath
			// 
			this->m_tb_filePath->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_tb_filePath->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_tb_filePath->Location = System::Drawing::Point(6, 6);
			this->m_tb_filePath->MaxLength = 256;
			this->m_tb_filePath->Name = L"m_tb_filePath";
			this->m_tb_filePath->Size = System::Drawing::Size(359, 22);
			this->m_tb_filePath->TabIndex = 0;
			this->m_tb_filePath->TextChanged += gcnew System::EventHandler(this, &SettingsGui::Settings_FilePath_TextChanged);
			// 
			// m_btn_saveChanges
			// 
			this->m_btn_saveChanges->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->m_btn_saveChanges->Enabled = false;
			this->m_btn_saveChanges->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_btn_saveChanges->Location = System::Drawing::Point(339, 314);
			this->m_btn_saveChanges->Name = L"m_btn_saveChanges";
			this->m_btn_saveChanges->Size = System::Drawing::Size(133, 35);
			this->m_btn_saveChanges->TabIndex = 1;
			this->m_btn_saveChanges->Text = L"Save Changes";
			this->m_btn_saveChanges->UseVisualStyleBackColor = true;
			// 
			// SettingsGui
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(484, 361);
			this->Controls->Add(this->m_btn_saveChanges);
			this->Controls->Add(this->m_tabControl);
			this->MinimumSize = System::Drawing::Size(500, 400);
			this->Name = L"SettingsGui";
			this->ShowIcon = false;
			this->Text = L"Settings";
			this->m_tabControl->ResumeLayout(false);
			this->m_tabPage_general->ResumeLayout(false);
			this->m_tabPage_general->PerformLayout();
			this->m_tabPage_paths->ResumeLayout(false);
			this->m_tabPage_paths->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
		System::Void Settings_PathSelector_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void Settings_FileOption_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);

		System::Void UpdatePathListFromMap(const std::unordered_map<std::wstring, unsigned char>& v_map);
		System::Void UpdatePathListFromVec(const std::vector<std::wstring>& v_path_list);
		System::Void UpdateCurrentPathList();

		System::Void Settings_PathList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void Settings_FilePath_TextChanged(System::Object^ sender, System::EventArgs^ e);

		std::wstring GetSelectedPathListString();
		System::Void Settings_RemovePath_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void Settings_AddPath_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void Settings_PathBrowser_Click(System::Object^ sender, System::EventArgs^ e);
	};
}

#pragma once

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

	private: System::Windows::Forms::TabControl^ m_tabControl;
		System::Windows::Forms::TabPage^ m_tabPage_general;
		System::Windows::Forms::TabPage^ m_tabPage_paths;
		System::Windows::Forms::Button^ m_btn_saveChanges;

		System::Windows::Forms::Label^ m_lbl_gamePath;
		System::Windows::Forms::CheckBox^ m_cb_openLinksInSteam;
		System::Windows::Forms::TextBox^ m_tb_gamePath;
		System::Windows::Forms::Button^ m_btn_gamePathSelector;

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
			this->m_btn_saveChanges = (gcnew System::Windows::Forms::Button());
			this->m_tabControl->SuspendLayout();
			this->m_tabPage_general->SuspendLayout();
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
			this->m_tabControl->Size = System::Drawing::Size(410, 196);
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
			this->m_tabPage_general->Size = System::Drawing::Size(402, 170);
			this->m_tabPage_general->TabIndex = 0;
			this->m_tabPage_general->Text = L"General";
			this->m_tabPage_general->UseVisualStyleBackColor = true;
			// 
			// m_btn_gamePathSelector
			// 
			this->m_btn_gamePathSelector->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->m_btn_gamePathSelector->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_btn_gamePathSelector->Location = System::Drawing::Point(356, 22);
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
			this->m_lbl_gamePath->Location = System::Drawing::Point(6, 3);
			this->m_lbl_gamePath->Name = L"m_lbl_gamePath";
			this->m_lbl_gamePath->Size = System::Drawing::Size(74, 16);
			this->m_lbl_gamePath->TabIndex = 2;
			this->m_lbl_gamePath->Text = L"Game Path";
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
			this->m_tb_gamePath->Size = System::Drawing::Size(344, 22);
			this->m_tb_gamePath->TabIndex = 0;
			// 
			// m_tabPage_paths
			// 
			this->m_tabPage_paths->Location = System::Drawing::Point(4, 22);
			this->m_tabPage_paths->Name = L"m_tabPage_paths";
			this->m_tabPage_paths->Padding = System::Windows::Forms::Padding(3);
			this->m_tabPage_paths->Size = System::Drawing::Size(402, 170);
			this->m_tabPage_paths->TabIndex = 1;
			this->m_tabPage_paths->Text = L"Paths";
			this->m_tabPage_paths->UseVisualStyleBackColor = true;
			// 
			// m_btn_saveChanges
			// 
			this->m_btn_saveChanges->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->m_btn_saveChanges->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_btn_saveChanges->Location = System::Drawing::Point(289, 214);
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
			this->ClientSize = System::Drawing::Size(434, 261);
			this->Controls->Add(this->m_btn_saveChanges);
			this->Controls->Add(this->m_tabControl);
			this->MinimumSize = System::Drawing::Size(450, 300);
			this->Name = L"SettingsGui";
			this->ShowIcon = false;
			this->Text = L"Settings";
			this->m_tabControl->ResumeLayout(false);
			this->m_tabPage_general->ResumeLayout(false);
			this->m_tabPage_general->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
		System::Void Settings_PathSelector_Click(System::Object^ sender, System::EventArgs^ e);
	};
}

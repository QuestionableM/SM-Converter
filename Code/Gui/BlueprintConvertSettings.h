#pragma once

namespace SMConverter
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class BlueprintConvertSettings : public System::Windows::Forms::Form
	{
	public:
		BlueprintConvertSettings(const wchar_t* v_filename);

		System::Windows::Forms::TextBox^ m_tb_filename;
		bool m_ready_to_convert = false;
		
		System::Windows::Forms::ComboBox^ m_cb_separationType;
		System::Windows::Forms::CheckBox^ m_cb_exportNormals;
		System::Windows::Forms::CheckBox^ m_cb_exportUvs;
		System::Windows::Forms::CheckBox^ m_cb_exportMaterials;

		System::Windows::Forms::ComboBox^ m_cb_customGame;

	protected:
		~BlueprintConvertSettings();

	private:
		System::Windows::Forms::GroupBox^ m_gb_customGameSettings;
		System::Windows::Forms::GroupBox^ m_gb_blueprintSettings;
		System::Windows::Forms::Label^ m_lbl_customGameContent;
		System::Windows::Forms::GroupBox^ m_gb_modelSettings;
		System::Windows::Forms::GroupBox^ m_gb_fileName;
		System::Windows::Forms::Button^ m_btn_convert;
		System::Windows::Forms::Label^ m_lbl_sepType;

		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->m_gb_fileName = (gcnew System::Windows::Forms::GroupBox());
			this->m_tb_filename = (gcnew System::Windows::Forms::TextBox());
			this->m_gb_blueprintSettings = (gcnew System::Windows::Forms::GroupBox());
			this->m_lbl_sepType = (gcnew System::Windows::Forms::Label());
			this->m_cb_separationType = (gcnew System::Windows::Forms::ComboBox());
			this->m_gb_modelSettings = (gcnew System::Windows::Forms::GroupBox());
			this->m_cb_exportNormals = (gcnew System::Windows::Forms::CheckBox());
			this->m_cb_exportUvs = (gcnew System::Windows::Forms::CheckBox());
			this->m_cb_exportMaterials = (gcnew System::Windows::Forms::CheckBox());
			this->m_btn_convert = (gcnew System::Windows::Forms::Button());
			this->m_gb_customGameSettings = (gcnew System::Windows::Forms::GroupBox());
			this->m_lbl_customGameContent = (gcnew System::Windows::Forms::Label());
			this->m_cb_customGame = (gcnew System::Windows::Forms::ComboBox());
			this->m_gb_fileName->SuspendLayout();
			this->m_gb_blueprintSettings->SuspendLayout();
			this->m_gb_modelSettings->SuspendLayout();
			this->m_gb_customGameSettings->SuspendLayout();
			this->SuspendLayout();
			// 
			// m_gb_fileName
			// 
			this->m_gb_fileName->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_gb_fileName->Controls->Add(this->m_tb_filename);
			this->m_gb_fileName->Location = System::Drawing::Point(12, 12);
			this->m_gb_fileName->Name = L"m_gb_fileName";
			this->m_gb_fileName->Size = System::Drawing::Size(310, 47);
			this->m_gb_fileName->TabIndex = 0;
			this->m_gb_fileName->TabStop = false;
			this->m_gb_fileName->Text = L"File Name";
			// 
			// m_tb_filename
			// 
			this->m_tb_filename->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_tb_filename->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_tb_filename->Location = System::Drawing::Point(6, 19);
			this->m_tb_filename->MaxLength = 42;
			this->m_tb_filename->Name = L"m_tb_filename";
			this->m_tb_filename->Size = System::Drawing::Size(298, 22);
			this->m_tb_filename->TabIndex = 0;
			this->m_tb_filename->TextChanged += gcnew System::EventHandler(this, &BlueprintConvertSettings::FileName_TextChanged);
			this->m_tb_filename->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &BlueprintConvertSettings::FileName_KeyPress);
			// 
			// m_gb_blueprintSettings
			// 
			this->m_gb_blueprintSettings->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_gb_blueprintSettings->Controls->Add(this->m_lbl_sepType);
			this->m_gb_blueprintSettings->Controls->Add(this->m_cb_separationType);
			this->m_gb_blueprintSettings->Location = System::Drawing::Point(12, 65);
			this->m_gb_blueprintSettings->Name = L"m_gb_blueprintSettings";
			this->m_gb_blueprintSettings->Size = System::Drawing::Size(310, 65);
			this->m_gb_blueprintSettings->TabIndex = 1;
			this->m_gb_blueprintSettings->TabStop = false;
			this->m_gb_blueprintSettings->Text = L"Blueprint Settings";
			// 
			// m_lbl_sepType
			// 
			this->m_lbl_sepType->AutoSize = true;
			this->m_lbl_sepType->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lbl_sepType->Location = System::Drawing::Point(3, 16);
			this->m_lbl_sepType->Name = L"m_lbl_sepType";
			this->m_lbl_sepType->Size = System::Drawing::Size(111, 16);
			this->m_lbl_sepType->TabIndex = 1;
			this->m_lbl_sepType->Text = L"Separation Type:";
			// 
			// m_cb_separationType
			// 
			this->m_cb_separationType->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_cb_separationType->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->m_cb_separationType->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_separationType->FormattingEnabled = true;
			this->m_cb_separationType->Items->AddRange(gcnew cli::array< System::Object^  >(6) {
				L"None", L"Separate All Objects", L"Separate By Joints",
					L"Group Objects By UUID", L"Group Objects By Color", L"Group Objects By UUID & Color"
			});
			this->m_cb_separationType->Location = System::Drawing::Point(6, 35);
			this->m_cb_separationType->Name = L"m_cb_separationType";
			this->m_cb_separationType->Size = System::Drawing::Size(298, 24);
			this->m_cb_separationType->TabIndex = 0;
			// 
			// m_gb_modelSettings
			// 
			this->m_gb_modelSettings->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_gb_modelSettings->Controls->Add(this->m_cb_exportNormals);
			this->m_gb_modelSettings->Controls->Add(this->m_cb_exportUvs);
			this->m_gb_modelSettings->Controls->Add(this->m_cb_exportMaterials);
			this->m_gb_modelSettings->Location = System::Drawing::Point(12, 136);
			this->m_gb_modelSettings->Name = L"m_gb_modelSettings";
			this->m_gb_modelSettings->Size = System::Drawing::Size(310, 97);
			this->m_gb_modelSettings->TabIndex = 2;
			this->m_gb_modelSettings->TabStop = false;
			this->m_gb_modelSettings->Text = L"Model Settings";
			// 
			// m_cb_exportNormals
			// 
			this->m_cb_exportNormals->AutoSize = true;
			this->m_cb_exportNormals->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportNormals->Location = System::Drawing::Point(6, 71);
			this->m_cb_exportNormals->Name = L"m_cb_exportNormals";
			this->m_cb_exportNormals->Size = System::Drawing::Size(118, 20);
			this->m_cb_exportNormals->TabIndex = 2;
			this->m_cb_exportNormals->Text = L"Export Normals";
			this->m_cb_exportNormals->UseVisualStyleBackColor = true;
			// 
			// m_cb_exportUvs
			// 
			this->m_cb_exportUvs->AutoSize = true;
			this->m_cb_exportUvs->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportUvs->Location = System::Drawing::Point(6, 45);
			this->m_cb_exportUvs->Name = L"m_cb_exportUvs";
			this->m_cb_exportUvs->Size = System::Drawing::Size(162, 20);
			this->m_cb_exportUvs->TabIndex = 1;
			this->m_cb_exportUvs->Text = L"Export UV Coordinates";
			this->m_cb_exportUvs->UseVisualStyleBackColor = true;
			this->m_cb_exportUvs->CheckedChanged += gcnew System::EventHandler(this, &BlueprintConvertSettings::ExportUvCoords_CheckedChanged);
			// 
			// m_cb_exportMaterials
			// 
			this->m_cb_exportMaterials->AutoSize = true;
			this->m_cb_exportMaterials->Enabled = false;
			this->m_cb_exportMaterials->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportMaterials->Location = System::Drawing::Point(6, 19);
			this->m_cb_exportMaterials->Name = L"m_cb_exportMaterials";
			this->m_cb_exportMaterials->Size = System::Drawing::Size(122, 20);
			this->m_cb_exportMaterials->TabIndex = 0;
			this->m_cb_exportMaterials->Text = L"Export Materials";
			this->m_cb_exportMaterials->UseVisualStyleBackColor = true;
			// 
			// m_btn_convert
			// 
			this->m_btn_convert->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_btn_convert->Enabled = false;
			this->m_btn_convert->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_btn_convert->Location = System::Drawing::Point(12, 310);
			this->m_btn_convert->Name = L"m_btn_convert";
			this->m_btn_convert->Size = System::Drawing::Size(310, 35);
			this->m_btn_convert->TabIndex = 3;
			this->m_btn_convert->Text = L"Convert";
			this->m_btn_convert->UseVisualStyleBackColor = true;
			this->m_btn_convert->Click += gcnew System::EventHandler(this, &BlueprintConvertSettings::ConvertButton_Click);
			// 
			// m_gb_customGameSettings
			// 
			this->m_gb_customGameSettings->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_gb_customGameSettings->Controls->Add(this->m_cb_customGame);
			this->m_gb_customGameSettings->Controls->Add(this->m_lbl_customGameContent);
			this->m_gb_customGameSettings->Location = System::Drawing::Point(12, 239);
			this->m_gb_customGameSettings->Name = L"m_gb_customGameSettings";
			this->m_gb_customGameSettings->Size = System::Drawing::Size(310, 65);
			this->m_gb_customGameSettings->TabIndex = 4;
			this->m_gb_customGameSettings->TabStop = false;
			this->m_gb_customGameSettings->Text = L"Custom Game Settings";
			// 
			// m_lbl_customGameContent
			// 
			this->m_lbl_customGameContent->AutoSize = true;
			this->m_lbl_customGameContent->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lbl_customGameContent->Location = System::Drawing::Point(3, 16);
			this->m_lbl_customGameContent->Name = L"m_lbl_customGameContent";
			this->m_lbl_customGameContent->Size = System::Drawing::Size(140, 16);
			this->m_lbl_customGameContent->TabIndex = 0;
			this->m_lbl_customGameContent->Text = L"Custom Game Content";
			// 
			// m_cb_customGame
			// 
			this->m_cb_customGame->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_cb_customGame->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->m_cb_customGame->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_customGame->FormattingEnabled = true;
			this->m_cb_customGame->Location = System::Drawing::Point(6, 35);
			this->m_cb_customGame->Name = L"m_cb_customGame";
			this->m_cb_customGame->Size = System::Drawing::Size(298, 24);
			this->m_cb_customGame->TabIndex = 1;
			// 
			// BlueprintConvertSettings
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(334, 357);
			this->Controls->Add(this->m_gb_customGameSettings);
			this->Controls->Add(this->m_btn_convert);
			this->Controls->Add(this->m_gb_modelSettings);
			this->Controls->Add(this->m_gb_blueprintSettings);
			this->Controls->Add(this->m_gb_fileName);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"BlueprintConvertSettings";
			this->ShowIcon = false;
			this->Text = L"Blueprint Convert Settings";
			this->m_gb_fileName->ResumeLayout(false);
			this->m_gb_fileName->PerformLayout();
			this->m_gb_blueprintSettings->ResumeLayout(false);
			this->m_gb_blueprintSettings->PerformLayout();
			this->m_gb_modelSettings->ResumeLayout(false);
			this->m_gb_modelSettings->PerformLayout();
			this->m_gb_customGameSettings->ResumeLayout(false);
			this->m_gb_customGameSettings->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
		System::Void FileName_TextChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void FileName_KeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e);
		System::Void ExportUvCoords_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void ConvertButton_Click(System::Object^ sender, System::EventArgs^ e);
	};
}

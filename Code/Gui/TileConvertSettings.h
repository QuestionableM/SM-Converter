#pragma once

namespace SMConverter
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for TileConvertSettings
	/// </summary>
	public ref class TileConvertSettings : public System::Windows::Forms::Form
	{
	public:
		TileConvertSettings(const wchar_t* v_filename);

	protected:
		~TileConvertSettings();

		//All the tile settings should be accessed in the MainGui
	public:
		System::Windows::Forms::TextBox^ m_tb_filename;
		bool m_ready_to_convert = false;
		
		//Tile Settings

		System::Windows::Forms::CheckBox^ m_cb_exportHarvestables;
		System::Windows::Forms::CheckBox^ m_cb_exportBlueprints;
		System::Windows::Forms::CheckBox^ m_cb_exportClutter;
		System::Windows::Forms::CheckBox^ m_cb_exportPrefabs;
		System::Windows::Forms::CheckBox^ m_cb_exportDecals;
		System::Windows::Forms::CheckBox^ m_cb_exportAssets;

		//Model Settings

		System::Windows::Forms::CheckBox^ m_cb_export8kGndTextures;
		System::Windows::Forms::CheckBox^ m_cb_exportGndTextures;
		System::Windows::Forms::CheckBox^ m_cb_exportMaterials;
		System::Windows::Forms::CheckBox^ m_cb_exportNormals;
		System::Windows::Forms::CheckBox^ m_cb_exportUvs;


	private:
		System::Windows::Forms::GroupBox^ m_gb_fileName;
		System::Windows::Forms::Button^ m_btn_convert;
		System::Windows::Forms::TabControl^ m_tc_settings;
		System::Windows::Forms::TabPage^ m_tab_tileSettings;
		System::Windows::Forms::TabPage^ m_tab_modelSettings;

		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->m_gb_fileName = (gcnew System::Windows::Forms::GroupBox());
			this->m_tb_filename = (gcnew System::Windows::Forms::TextBox());
			this->m_cb_exportDecals = (gcnew System::Windows::Forms::CheckBox());
			this->m_cb_exportHarvestables = (gcnew System::Windows::Forms::CheckBox());
			this->m_cb_exportBlueprints = (gcnew System::Windows::Forms::CheckBox());
			this->m_cb_exportPrefabs = (gcnew System::Windows::Forms::CheckBox());
			this->m_cb_exportAssets = (gcnew System::Windows::Forms::CheckBox());
			this->m_cb_exportClutter = (gcnew System::Windows::Forms::CheckBox());
			this->m_btn_convert = (gcnew System::Windows::Forms::Button());
			this->m_cb_exportGndTextures = (gcnew System::Windows::Forms::CheckBox());
			this->m_cb_export8kGndTextures = (gcnew System::Windows::Forms::CheckBox());
			this->m_cb_exportMaterials = (gcnew System::Windows::Forms::CheckBox());
			this->m_cb_exportUvs = (gcnew System::Windows::Forms::CheckBox());
			this->m_cb_exportNormals = (gcnew System::Windows::Forms::CheckBox());
			this->m_tc_settings = (gcnew System::Windows::Forms::TabControl());
			this->m_tab_tileSettings = (gcnew System::Windows::Forms::TabPage());
			this->m_tab_modelSettings = (gcnew System::Windows::Forms::TabPage());
			this->m_gb_fileName->SuspendLayout();
			this->m_tc_settings->SuspendLayout();
			this->m_tab_tileSettings->SuspendLayout();
			this->m_tab_modelSettings->SuspendLayout();
			this->SuspendLayout();
			// 
			// m_gb_fileName
			// 
			this->m_gb_fileName->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_gb_fileName->Controls->Add(this->m_tb_filename);
			this->m_gb_fileName->Location = System::Drawing::Point(12, 12);
			this->m_gb_fileName->Name = L"m_gb_fileName";
			this->m_gb_fileName->Size = System::Drawing::Size(190, 47);
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
			this->m_tb_filename->Name = L"m_tb_filename";
			this->m_tb_filename->Size = System::Drawing::Size(178, 22);
			this->m_tb_filename->TabIndex = 0;
			this->m_tb_filename->TextChanged += gcnew System::EventHandler(this, &TileConvertSettings::FileName_TextChanged);
			// 
			// m_cb_exportDecals
			// 
			this->m_cb_exportDecals->AutoSize = true;
			this->m_cb_exportDecals->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportDecals->Location = System::Drawing::Point(6, 136);
			this->m_cb_exportDecals->Name = L"m_cb_exportDecals";
			this->m_cb_exportDecals->Size = System::Drawing::Size(146, 20);
			this->m_cb_exportDecals->TabIndex = 5;
			this->m_cb_exportDecals->Text = L"Export Decals (WIP)";
			this->m_cb_exportDecals->UseVisualStyleBackColor = true;
			// 
			// m_cb_exportHarvestables
			// 
			this->m_cb_exportHarvestables->AutoSize = true;
			this->m_cb_exportHarvestables->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportHarvestables->Location = System::Drawing::Point(6, 110);
			this->m_cb_exportHarvestables->Name = L"m_cb_exportHarvestables";
			this->m_cb_exportHarvestables->Size = System::Drawing::Size(148, 20);
			this->m_cb_exportHarvestables->TabIndex = 4;
			this->m_cb_exportHarvestables->Text = L"Export Harvestables";
			this->m_cb_exportHarvestables->UseVisualStyleBackColor = true;
			// 
			// m_cb_exportBlueprints
			// 
			this->m_cb_exportBlueprints->AutoSize = true;
			this->m_cb_exportBlueprints->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportBlueprints->Location = System::Drawing::Point(6, 84);
			this->m_cb_exportBlueprints->Name = L"m_cb_exportBlueprints";
			this->m_cb_exportBlueprints->Size = System::Drawing::Size(126, 20);
			this->m_cb_exportBlueprints->TabIndex = 3;
			this->m_cb_exportBlueprints->Text = L"Export Blueprints";
			this->m_cb_exportBlueprints->UseVisualStyleBackColor = true;
			// 
			// m_cb_exportPrefabs
			// 
			this->m_cb_exportPrefabs->AutoSize = true;
			this->m_cb_exportPrefabs->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportPrefabs->Location = System::Drawing::Point(6, 58);
			this->m_cb_exportPrefabs->Name = L"m_cb_exportPrefabs";
			this->m_cb_exportPrefabs->Size = System::Drawing::Size(114, 20);
			this->m_cb_exportPrefabs->TabIndex = 2;
			this->m_cb_exportPrefabs->Text = L"Export Prefabs";
			this->m_cb_exportPrefabs->UseVisualStyleBackColor = true;
			// 
			// m_cb_exportAssets
			// 
			this->m_cb_exportAssets->AutoSize = true;
			this->m_cb_exportAssets->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportAssets->Location = System::Drawing::Point(6, 32);
			this->m_cb_exportAssets->Name = L"m_cb_exportAssets";
			this->m_cb_exportAssets->Size = System::Drawing::Size(108, 20);
			this->m_cb_exportAssets->TabIndex = 1;
			this->m_cb_exportAssets->Text = L"Export Assets";
			this->m_cb_exportAssets->UseVisualStyleBackColor = true;
			// 
			// m_cb_exportClutter
			// 
			this->m_cb_exportClutter->AutoSize = true;
			this->m_cb_exportClutter->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportClutter->Location = System::Drawing::Point(6, 6);
			this->m_cb_exportClutter->Name = L"m_cb_exportClutter";
			this->m_cb_exportClutter->Size = System::Drawing::Size(104, 20);
			this->m_cb_exportClutter->TabIndex = 0;
			this->m_cb_exportClutter->Text = L"Export Clutter";
			this->m_cb_exportClutter->UseVisualStyleBackColor = true;
			// 
			// m_btn_convert
			// 
			this->m_btn_convert->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_btn_convert->Enabled = false;
			this->m_btn_convert->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_btn_convert->Location = System::Drawing::Point(12, 258);
			this->m_btn_convert->Name = L"m_btn_convert";
			this->m_btn_convert->Size = System::Drawing::Size(190, 40);
			this->m_btn_convert->TabIndex = 3;
			this->m_btn_convert->Text = L"Convert";
			this->m_btn_convert->UseVisualStyleBackColor = true;
			this->m_btn_convert->Click += gcnew System::EventHandler(this, &TileConvertSettings::ConvertButton_Click);
			// 
			// m_cb_exportGndTextures
			// 
			this->m_cb_exportGndTextures->AutoSize = true;
			this->m_cb_exportGndTextures->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportGndTextures->Location = System::Drawing::Point(6, 32);
			this->m_cb_exportGndTextures->Name = L"m_cb_exportGndTextures";
			this->m_cb_exportGndTextures->Size = System::Drawing::Size(166, 20);
			this->m_cb_exportGndTextures->TabIndex = 6;
			this->m_cb_exportGndTextures->Text = L"Export Ground Textures";
			this->m_cb_exportGndTextures->UseVisualStyleBackColor = true;
			// 
			// m_cb_export8kGndTextures
			// 
			this->m_cb_export8kGndTextures->AutoSize = true;
			this->m_cb_export8kGndTextures->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_export8kGndTextures->Location = System::Drawing::Point(6, 58);
			this->m_cb_export8kGndTextures->Name = L"m_cb_export8kGndTextures";
			this->m_cb_export8kGndTextures->Size = System::Drawing::Size(143, 20);
			this->m_cb_export8kGndTextures->TabIndex = 7;
			this->m_cb_export8kGndTextures->Text = L"8K Ground Textures";
			this->m_cb_export8kGndTextures->UseVisualStyleBackColor = true;
			// 
			// m_cb_exportMaterials
			// 
			this->m_cb_exportMaterials->AutoSize = true;
			this->m_cb_exportMaterials->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportMaterials->Location = System::Drawing::Point(6, 6);
			this->m_cb_exportMaterials->Name = L"m_cb_exportMaterials";
			this->m_cb_exportMaterials->Size = System::Drawing::Size(122, 20);
			this->m_cb_exportMaterials->TabIndex = 0;
			this->m_cb_exportMaterials->Text = L"Export Materials";
			this->m_cb_exportMaterials->UseVisualStyleBackColor = true;
			// 
			// m_cb_exportUvs
			// 
			this->m_cb_exportUvs->AutoSize = true;
			this->m_cb_exportUvs->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportUvs->Location = System::Drawing::Point(6, 84);
			this->m_cb_exportUvs->Name = L"m_cb_exportUvs";
			this->m_cb_exportUvs->Size = System::Drawing::Size(162, 20);
			this->m_cb_exportUvs->TabIndex = 1;
			this->m_cb_exportUvs->Text = L"Export UV Coordinates";
			this->m_cb_exportUvs->UseVisualStyleBackColor = true;
			// 
			// m_cb_exportNormals
			// 
			this->m_cb_exportNormals->AutoSize = true;
			this->m_cb_exportNormals->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportNormals->Location = System::Drawing::Point(6, 110);
			this->m_cb_exportNormals->Name = L"m_cb_exportNormals";
			this->m_cb_exportNormals->Size = System::Drawing::Size(118, 20);
			this->m_cb_exportNormals->TabIndex = 2;
			this->m_cb_exportNormals->Text = L"Export Normals";
			this->m_cb_exportNormals->UseVisualStyleBackColor = true;
			// 
			// m_tc_settings
			// 
			this->m_tc_settings->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_tc_settings->Controls->Add(this->m_tab_tileSettings);
			this->m_tc_settings->Controls->Add(this->m_tab_modelSettings);
			this->m_tc_settings->Location = System::Drawing::Point(12, 65);
			this->m_tc_settings->Name = L"m_tc_settings";
			this->m_tc_settings->SelectedIndex = 0;
			this->m_tc_settings->Size = System::Drawing::Size(190, 187);
			this->m_tc_settings->TabIndex = 8;
			// 
			// m_tab_tileSettings
			// 
			this->m_tab_tileSettings->Controls->Add(this->m_cb_exportClutter);
			this->m_tab_tileSettings->Controls->Add(this->m_cb_exportAssets);
			this->m_tab_tileSettings->Controls->Add(this->m_cb_exportDecals);
			this->m_tab_tileSettings->Controls->Add(this->m_cb_exportPrefabs);
			this->m_tab_tileSettings->Controls->Add(this->m_cb_exportHarvestables);
			this->m_tab_tileSettings->Controls->Add(this->m_cb_exportBlueprints);
			this->m_tab_tileSettings->Location = System::Drawing::Point(4, 22);
			this->m_tab_tileSettings->Name = L"m_tab_tileSettings";
			this->m_tab_tileSettings->Padding = System::Windows::Forms::Padding(3);
			this->m_tab_tileSettings->Size = System::Drawing::Size(182, 161);
			this->m_tab_tileSettings->TabIndex = 0;
			this->m_tab_tileSettings->Text = L"Tile Settings";
			this->m_tab_tileSettings->UseVisualStyleBackColor = true;
			// 
			// m_tab_modelSettings
			// 
			this->m_tab_modelSettings->Controls->Add(this->m_cb_exportNormals);
			this->m_tab_modelSettings->Controls->Add(this->m_cb_export8kGndTextures);
			this->m_tab_modelSettings->Controls->Add(this->m_cb_exportUvs);
			this->m_tab_modelSettings->Controls->Add(this->m_cb_exportMaterials);
			this->m_tab_modelSettings->Controls->Add(this->m_cb_exportGndTextures);
			this->m_tab_modelSettings->Location = System::Drawing::Point(4, 22);
			this->m_tab_modelSettings->Name = L"m_tab_modelSettings";
			this->m_tab_modelSettings->Padding = System::Windows::Forms::Padding(3);
			this->m_tab_modelSettings->Size = System::Drawing::Size(182, 161);
			this->m_tab_modelSettings->TabIndex = 1;
			this->m_tab_modelSettings->Text = L"Model Settings";
			this->m_tab_modelSettings->UseVisualStyleBackColor = true;
			// 
			// TileConvertSettings
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(214, 310);
			this->Controls->Add(this->m_tc_settings);
			this->Controls->Add(this->m_btn_convert);
			this->Controls->Add(this->m_gb_fileName);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"TileConvertSettings";
			this->ShowIcon = false;
			this->Text = L"Tile Convert Settings";
			this->m_gb_fileName->ResumeLayout(false);
			this->m_gb_fileName->PerformLayout();
			this->m_tc_settings->ResumeLayout(false);
			this->m_tab_tileSettings->ResumeLayout(false);
			this->m_tab_tileSettings->PerformLayout();
			this->m_tab_modelSettings->ResumeLayout(false);
			this->m_tab_modelSettings->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
		System::Void FileName_TextChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void ConvertButton_Click(System::Object^ sender, System::EventArgs^ e);
	};
}

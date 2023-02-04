#pragma once

#include "Utils\Uuid.hpp"

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
		TileConvertSettings(const wchar_t* v_filename, SMUuid* v_cg_uuid);

	protected:
		~TileConvertSettings();

		//All the tile settings should be accessed in the MainGui
	public:
		System::Windows::Forms::TextBox^ m_tb_filename;
		bool m_ready_to_convert = false;
		
		//Tile Settings

		System::Windows::Forms::CheckBox^ m_cb_exportHarvestables;
		System::Windows::Forms::CheckBox^ m_cb_exportBlueprints;
		System::Windows::Forms::CheckBox^ m_cb_exportKinematics;
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

		//Selected custom game content

		System::Windows::Forms::ComboBox^ m_cb_customGame;

	private:
		System::Windows::Forms::GroupBox^ m_gb_fileName;
		System::Windows::Forms::Button^ m_btn_convert;
		System::Windows::Forms::GroupBox^ m_gb_tileSettings;
		System::Windows::Forms::GroupBox^ m_gb_modelSettings;
		System::Windows::Forms::GroupBox^ m_gb_customGameSettings;
		System::Windows::Forms::Label^ m_lbl_customGameContent;
		System::Windows::Forms::ToolTip^ m_toolTip;

		System::ComponentModel::IContainer^ components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
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
			this->m_gb_tileSettings = (gcnew System::Windows::Forms::GroupBox());
			this->m_gb_modelSettings = (gcnew System::Windows::Forms::GroupBox());
			this->m_gb_customGameSettings = (gcnew System::Windows::Forms::GroupBox());
			this->m_cb_customGame = (gcnew System::Windows::Forms::ComboBox());
			this->m_lbl_customGameContent = (gcnew System::Windows::Forms::Label());
			this->m_toolTip = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->m_cb_exportKinematics = (gcnew System::Windows::Forms::CheckBox());
			this->m_gb_fileName->SuspendLayout();
			this->m_gb_tileSettings->SuspendLayout();
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
			this->m_gb_fileName->Size = System::Drawing::Size(366, 47);
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
			this->m_tb_filename->Size = System::Drawing::Size(354, 22);
			this->m_tb_filename->TabIndex = 0;
			this->m_tb_filename->TextChanged += gcnew System::EventHandler(this, &TileConvertSettings::FileName_TextChanged);
			this->m_tb_filename->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &TileConvertSettings::FileName_KeyPress);
			// 
			// m_cb_exportDecals
			// 
			this->m_cb_exportDecals->AutoSize = true;
			this->m_cb_exportDecals->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportDecals->Location = System::Drawing::Point(6, 175);
			this->m_cb_exportDecals->Name = L"m_cb_exportDecals";
			this->m_cb_exportDecals->Size = System::Drawing::Size(146, 20);
			this->m_cb_exportDecals->TabIndex = 5;
			this->m_cb_exportDecals->Text = L"Export Decals (WIP)";
			this->m_toolTip->SetToolTip(this->m_cb_exportDecals, L"This function is still in development");
			this->m_cb_exportDecals->UseVisualStyleBackColor = true;
			// 
			// m_cb_exportHarvestables
			// 
			this->m_cb_exportHarvestables->AutoSize = true;
			this->m_cb_exportHarvestables->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportHarvestables->Location = System::Drawing::Point(6, 149);
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
			this->m_cb_exportBlueprints->Location = System::Drawing::Point(6, 97);
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
			this->m_cb_exportPrefabs->Location = System::Drawing::Point(6, 71);
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
			this->m_cb_exportAssets->Location = System::Drawing::Point(6, 45);
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
			this->m_cb_exportClutter->Location = System::Drawing::Point(6, 19);
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
			this->m_btn_convert->Location = System::Drawing::Point(12, 343);
			this->m_btn_convert->Name = L"m_btn_convert";
			this->m_btn_convert->Size = System::Drawing::Size(366, 35);
			this->m_btn_convert->TabIndex = 3;
			this->m_btn_convert->Text = L"Convert";
			this->m_btn_convert->UseVisualStyleBackColor = true;
			this->m_btn_convert->Click += gcnew System::EventHandler(this, &TileConvertSettings::ConvertButton_Click);
			// 
			// m_cb_exportGndTextures
			// 
			this->m_cb_exportGndTextures->AutoSize = true;
			this->m_cb_exportGndTextures->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportGndTextures->Location = System::Drawing::Point(6, 45);
			this->m_cb_exportGndTextures->Name = L"m_cb_exportGndTextures";
			this->m_cb_exportGndTextures->Size = System::Drawing::Size(166, 20);
			this->m_cb_exportGndTextures->TabIndex = 6;
			this->m_cb_exportGndTextures->Text = L"Export Ground Textures";
			this->m_toolTip->SetToolTip(this->m_cb_exportGndTextures, L"When checked, the tool will export 3 ground textures in 4k resolution");
			this->m_cb_exportGndTextures->UseVisualStyleBackColor = true;
			this->m_cb_exportGndTextures->CheckedChanged += gcnew System::EventHandler(this, &TileConvertSettings::ExportGroundTextures_CheckedChanged);
			// 
			// m_cb_export8kGndTextures
			// 
			this->m_cb_export8kGndTextures->AutoSize = true;
			this->m_cb_export8kGndTextures->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_export8kGndTextures->Location = System::Drawing::Point(6, 71);
			this->m_cb_export8kGndTextures->Name = L"m_cb_export8kGndTextures";
			this->m_cb_export8kGndTextures->Size = System::Drawing::Size(143, 20);
			this->m_cb_export8kGndTextures->TabIndex = 7;
			this->m_cb_export8kGndTextures->Text = L"8K Ground Textures";
			this->m_toolTip->SetToolTip(this->m_cb_export8kGndTextures, L"Export ground textures in 8k resolution");
			this->m_cb_export8kGndTextures->UseVisualStyleBackColor = true;
			// 
			// m_cb_exportMaterials
			// 
			this->m_cb_exportMaterials->AutoSize = true;
			this->m_cb_exportMaterials->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportMaterials->Location = System::Drawing::Point(6, 19);
			this->m_cb_exportMaterials->Name = L"m_cb_exportMaterials";
			this->m_cb_exportMaterials->Size = System::Drawing::Size(122, 20);
			this->m_cb_exportMaterials->TabIndex = 0;
			this->m_cb_exportMaterials->Text = L"Export Materials";
			this->m_toolTip->SetToolTip(this->m_cb_exportMaterials, L"When checked, the exported model will be linked to an .mtl file, which contains\r\n"
				L"the information about object materials");
			this->m_cb_exportMaterials->UseVisualStyleBackColor = true;
			// 
			// m_cb_exportUvs
			// 
			this->m_cb_exportUvs->AutoSize = true;
			this->m_cb_exportUvs->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportUvs->Location = System::Drawing::Point(6, 97);
			this->m_cb_exportUvs->Name = L"m_cb_exportUvs";
			this->m_cb_exportUvs->Size = System::Drawing::Size(162, 20);
			this->m_cb_exportUvs->TabIndex = 1;
			this->m_cb_exportUvs->Text = L"Export UV Coordinates";
			this->m_toolTip->SetToolTip(this->m_cb_exportUvs, L"When checked, the exported model will contain UV coordinates");
			this->m_cb_exportUvs->UseVisualStyleBackColor = true;
			this->m_cb_exportUvs->CheckedChanged += gcnew System::EventHandler(this, &TileConvertSettings::ExportUvCoords_CheckedChanged);
			// 
			// m_cb_exportNormals
			// 
			this->m_cb_exportNormals->AutoSize = true;
			this->m_cb_exportNormals->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportNormals->Location = System::Drawing::Point(6, 123);
			this->m_cb_exportNormals->Name = L"m_cb_exportNormals";
			this->m_cb_exportNormals->Size = System::Drawing::Size(118, 20);
			this->m_cb_exportNormals->TabIndex = 2;
			this->m_cb_exportNormals->Text = L"Export Normals";
			this->m_toolTip->SetToolTip(this->m_cb_exportNormals, L"When checked, the exported model will contain the information about the normals");
			this->m_cb_exportNormals->UseVisualStyleBackColor = true;
			// 
			// m_gb_tileSettings
			// 
			this->m_gb_tileSettings->Controls->Add(this->m_cb_exportKinematics);
			this->m_gb_tileSettings->Controls->Add(this->m_cb_exportDecals);
			this->m_gb_tileSettings->Controls->Add(this->m_cb_exportAssets);
			this->m_gb_tileSettings->Controls->Add(this->m_cb_exportHarvestables);
			this->m_gb_tileSettings->Controls->Add(this->m_cb_exportPrefabs);
			this->m_gb_tileSettings->Controls->Add(this->m_cb_exportBlueprints);
			this->m_gb_tileSettings->Controls->Add(this->m_cb_exportClutter);
			this->m_gb_tileSettings->Location = System::Drawing::Point(12, 65);
			this->m_gb_tileSettings->Name = L"m_gb_tileSettings";
			this->m_gb_tileSettings->Size = System::Drawing::Size(180, 201);
			this->m_gb_tileSettings->TabIndex = 9;
			this->m_gb_tileSettings->TabStop = false;
			this->m_gb_tileSettings->Text = L"Tile Settings";
			// 
			// m_gb_modelSettings
			// 
			this->m_gb_modelSettings->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->m_gb_modelSettings->Controls->Add(this->m_cb_exportNormals);
			this->m_gb_modelSettings->Controls->Add(this->m_cb_exportMaterials);
			this->m_gb_modelSettings->Controls->Add(this->m_cb_exportUvs);
			this->m_gb_modelSettings->Controls->Add(this->m_cb_export8kGndTextures);
			this->m_gb_modelSettings->Controls->Add(this->m_cb_exportGndTextures);
			this->m_gb_modelSettings->Location = System::Drawing::Point(198, 65);
			this->m_gb_modelSettings->Name = L"m_gb_modelSettings";
			this->m_gb_modelSettings->Size = System::Drawing::Size(180, 201);
			this->m_gb_modelSettings->TabIndex = 10;
			this->m_gb_modelSettings->TabStop = false;
			this->m_gb_modelSettings->Text = L"Model Settings";
			// 
			// m_gb_customGameSettings
			// 
			this->m_gb_customGameSettings->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_gb_customGameSettings->Controls->Add(this->m_cb_customGame);
			this->m_gb_customGameSettings->Controls->Add(this->m_lbl_customGameContent);
			this->m_gb_customGameSettings->Location = System::Drawing::Point(12, 272);
			this->m_gb_customGameSettings->Name = L"m_gb_customGameSettings";
			this->m_gb_customGameSettings->Size = System::Drawing::Size(366, 65);
			this->m_gb_customGameSettings->TabIndex = 11;
			this->m_gb_customGameSettings->TabStop = false;
			this->m_gb_customGameSettings->Text = L"Custom Game Settings";
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
			this->m_cb_customGame->Size = System::Drawing::Size(354, 24);
			this->m_cb_customGame->TabIndex = 1;
			this->m_toolTip->SetToolTip(this->m_cb_customGame, L"Selected custom game\r\n\r\nWhen None is selected, the converter will use the content"
				L" from base game\r\nand workshop mods\r\n\r\nOtherwise the tool will load the content f"
				L"rom a custom game");
			// 
			// m_lbl_customGameContent
			// 
			this->m_lbl_customGameContent->AutoSize = true;
			this->m_lbl_customGameContent->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lbl_customGameContent->Location = System::Drawing::Point(3, 16);
			this->m_lbl_customGameContent->Name = L"m_lbl_customGameContent";
			this->m_lbl_customGameContent->Size = System::Drawing::Size(143, 16);
			this->m_lbl_customGameContent->TabIndex = 0;
			this->m_lbl_customGameContent->Text = L"Custom Game Content:";
			// 
			// m_cb_exportKinematics
			// 
			this->m_cb_exportKinematics->AutoSize = true;
			this->m_cb_exportKinematics->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportKinematics->Location = System::Drawing::Point(6, 123);
			this->m_cb_exportKinematics->Name = L"m_cb_exportKinematics";
			this->m_cb_exportKinematics->Size = System::Drawing::Size(132, 20);
			this->m_cb_exportKinematics->TabIndex = 6;
			this->m_cb_exportKinematics->Text = L"Export Kinematics";
			this->m_cb_exportKinematics->UseVisualStyleBackColor = true;
			// 
			// TileConvertSettings
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(390, 390);
			this->Controls->Add(this->m_gb_customGameSettings);
			this->Controls->Add(this->m_gb_modelSettings);
			this->Controls->Add(this->m_gb_tileSettings);
			this->Controls->Add(this->m_btn_convert);
			this->Controls->Add(this->m_gb_fileName);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->MinimumSize = System::Drawing::Size(406, 403);
			this->Name = L"TileConvertSettings";
			this->ShowIcon = false;
			this->Text = L"Tile Convert Settings";
			this->m_gb_fileName->ResumeLayout(false);
			this->m_gb_fileName->PerformLayout();
			this->m_gb_tileSettings->ResumeLayout(false);
			this->m_gb_tileSettings->PerformLayout();
			this->m_gb_modelSettings->ResumeLayout(false);
			this->m_gb_modelSettings->PerformLayout();
			this->m_gb_customGameSettings->ResumeLayout(false);
			this->m_gb_customGameSettings->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
		System::Void FileName_TextChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void FileName_KeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e);

		System::Void ConvertButton_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void ExportGroundTextures_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void ExportUvCoords_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
	};
}

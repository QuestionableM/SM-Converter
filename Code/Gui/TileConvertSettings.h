#pragma once

namespace SMConverter {

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
		TileConvertSettings(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~TileConvertSettings()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^ m_gb_fileName;
	protected:
	private: System::Windows::Forms::TextBox^ m_tb_filename;
	private: System::Windows::Forms::GroupBox^ m_gb_tileSettings;
	private: System::Windows::Forms::GroupBox^ m_gb_modelSettings;
	private: System::Windows::Forms::Button^ m_btn_convert;
	private: System::Windows::Forms::CheckBox^ m_cb_exportDecals;

	private: System::Windows::Forms::CheckBox^ m_cb_exportHarvestables;

	private: System::Windows::Forms::CheckBox^ m_cb_exportBlueprints;

	private: System::Windows::Forms::CheckBox^ m_cb_exportPrefabs;

	private: System::Windows::Forms::CheckBox^ m_cb_exportAssets;

	private: System::Windows::Forms::CheckBox^ m_cb_exportClutter;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->m_gb_fileName = (gcnew System::Windows::Forms::GroupBox());
			this->m_tb_filename = (gcnew System::Windows::Forms::TextBox());
			this->m_gb_tileSettings = (gcnew System::Windows::Forms::GroupBox());
			this->m_cb_exportDecals = (gcnew System::Windows::Forms::CheckBox());
			this->m_cb_exportHarvestables = (gcnew System::Windows::Forms::CheckBox());
			this->m_cb_exportBlueprints = (gcnew System::Windows::Forms::CheckBox());
			this->m_cb_exportPrefabs = (gcnew System::Windows::Forms::CheckBox());
			this->m_cb_exportAssets = (gcnew System::Windows::Forms::CheckBox());
			this->m_cb_exportClutter = (gcnew System::Windows::Forms::CheckBox());
			this->m_gb_modelSettings = (gcnew System::Windows::Forms::GroupBox());
			this->m_btn_convert = (gcnew System::Windows::Forms::Button());
			this->m_gb_fileName->SuspendLayout();
			this->m_gb_tileSettings->SuspendLayout();
			this->SuspendLayout();
			// 
			// m_gb_fileName
			// 
			this->m_gb_fileName->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_gb_fileName->Controls->Add(this->m_tb_filename);
			this->m_gb_fileName->Location = System::Drawing::Point(12, 12);
			this->m_gb_fileName->Name = L"m_gb_fileName";
			this->m_gb_fileName->Size = System::Drawing::Size(199, 47);
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
			this->m_tb_filename->Size = System::Drawing::Size(187, 22);
			this->m_tb_filename->TabIndex = 0;
			// 
			// m_gb_tileSettings
			// 
			this->m_gb_tileSettings->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_gb_tileSettings->Controls->Add(this->m_cb_exportDecals);
			this->m_gb_tileSettings->Controls->Add(this->m_cb_exportHarvestables);
			this->m_gb_tileSettings->Controls->Add(this->m_cb_exportBlueprints);
			this->m_gb_tileSettings->Controls->Add(this->m_cb_exportPrefabs);
			this->m_gb_tileSettings->Controls->Add(this->m_cb_exportAssets);
			this->m_gb_tileSettings->Controls->Add(this->m_cb_exportClutter);
			this->m_gb_tileSettings->Location = System::Drawing::Point(12, 65);
			this->m_gb_tileSettings->Name = L"m_gb_tileSettings";
			this->m_gb_tileSettings->Size = System::Drawing::Size(199, 177);
			this->m_gb_tileSettings->TabIndex = 1;
			this->m_gb_tileSettings->TabStop = false;
			this->m_gb_tileSettings->Text = L"Tile Settings";
			// 
			// m_cb_exportDecals
			// 
			this->m_cb_exportDecals->AutoSize = true;
			this->m_cb_exportDecals->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_exportDecals->Location = System::Drawing::Point(6, 149);
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
			this->m_cb_exportHarvestables->Location = System::Drawing::Point(6, 123);
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
			// m_gb_modelSettings
			// 
			this->m_gb_modelSettings->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_gb_modelSettings->Location = System::Drawing::Point(12, 261);
			this->m_gb_modelSettings->Name = L"m_gb_modelSettings";
			this->m_gb_modelSettings->Size = System::Drawing::Size(199, 100);
			this->m_gb_modelSettings->TabIndex = 2;
			this->m_gb_modelSettings->TabStop = false;
			this->m_gb_modelSettings->Text = L"Model Settings";
			// 
			// m_btn_convert
			// 
			this->m_btn_convert->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_btn_convert->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_btn_convert->Location = System::Drawing::Point(12, 367);
			this->m_btn_convert->Name = L"m_btn_convert";
			this->m_btn_convert->Size = System::Drawing::Size(199, 40);
			this->m_btn_convert->TabIndex = 3;
			this->m_btn_convert->Text = L"Convert";
			this->m_btn_convert->UseVisualStyleBackColor = true;
			// 
			// TileConvertSettings
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(223, 419);
			this->Controls->Add(this->m_btn_convert);
			this->Controls->Add(this->m_gb_modelSettings);
			this->Controls->Add(this->m_gb_tileSettings);
			this->Controls->Add(this->m_gb_fileName);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"TileConvertSettings";
			this->ShowIcon = false;
			this->Text = L"Tile Convert Settings";
			this->m_gb_fileName->ResumeLayout(false);
			this->m_gb_fileName->PerformLayout();
			this->m_gb_tileSettings->ResumeLayout(false);
			this->m_gb_tileSettings->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	};
}

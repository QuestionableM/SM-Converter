#pragma once

#include "ObjectDatabase\UserDataReaders\BlueprintFolderReader.hpp"
#include "ObjectDatabase\UserDataReaders\WorldFolderReader.hpp"
#include "ObjectDatabase\UserDataReaders\TileFolderReader.hpp"

#include "Converter\ConvertError.hpp"
#include "UStd\UnmanagedVector.hpp"

namespace SMConverter
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class MainGui : public System::Windows::Forms::Form
	{
	public:
		MainGui(void);

	protected:
		~MainGui();

	private:
		bool m_obj_isLoaded = false;
		bool m_database_isLoaded = false;
		int m_lastSearchLength = 0;

		System::Windows::Forms::Button^ m_btn_folderDialog;
		System::Windows::Forms::Button^ m_btn_searchFilter;
		System::Windows::Forms::Button^ m_btn_convert;

		System::Windows::Forms::ToolStripMenuItem^ m_btn_reloadDatabase;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_options;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_reloadUserObjects;
		System::Windows::Forms::ToolStripMenuItem^ m_menuItem_settings;

		System::Windows::Forms::TextBox^ m_tb_path;
		System::Windows::Forms::TextBox^ m_tb_searchBox;
		System::Windows::Forms::ListBox^ m_lb_objectSelector;
		System::Windows::Forms::ComboBox^ m_cb_selectedGenerator;

		System::Windows::Forms::ContextMenuStrip^ m_cms_blueprint;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_openBlueprintFolder;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_openBlueprintInSteamWorkshop;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_showBlueprintInfo;

		System::Windows::Forms::Label^ m_lbl_generatorType;
		System::Windows::Forms::Label^ m_lbl_progressStatus;
		System::Windows::Forms::Label^ m_lbl_objSelectorStatus;

		System::ComponentModel::BackgroundWorker^ m_bw_objectConverter;
		System::ComponentModel::BackgroundWorker^ m_bw_databaseLoader;
		System::ComponentModel::BackgroundWorker^ m_bw_objectLoader;

		System::Windows::Forms::ProgressBar^ m_pb_progress;
		System::Windows::Forms::MenuStrip^ m_menuStrip;
		System::Windows::Forms::Timer^ m_progressBarUpdater;
		
		System::Windows::Forms::ContextMenuStrip^ m_cms_tile;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_openTileInSteamWorkshop;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_findTileCreatorInSteam;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_openTileFolder;
		System::Windows::Forms::ToolStripSeparator^ m_cms_tile_separator;
		System::Windows::Forms::ToolStripSeparator^ m_cms_tile_separator2;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_showTileInfo;

		System::Windows::Forms::ToolStripMenuItem^ m_menuItem_open;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_openBlueprintOutputDirectory;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_openWorldOutputDirectory;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_openTileOutputDirectory;
		System::Windows::Forms::ToolStripSeparator^ m_ts_open_separator;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_aboutProgram;

		System::Windows::Forms::ToolStripSeparator^ m_ts_settings_separator;
		System::Windows::Forms::ToolStripSeparator^ m_cms_toolStrip;
		System::Windows::Forms::ToolTip^ m_toolTip;

		System::Windows::Forms::ContextMenuStrip^ m_cms_world;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_openWorldInSteamWorkshop;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_openWorldFolder;
		System::Windows::Forms::ToolStripSeparator^ toolStripSeparator1;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_showWorldInfo;

		System::ComponentModel::IContainer^ components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->m_btn_folderDialog = (gcnew System::Windows::Forms::Button());
			this->m_btn_convert = (gcnew System::Windows::Forms::Button());
			this->m_tb_path = (gcnew System::Windows::Forms::TextBox());
			this->m_tb_searchBox = (gcnew System::Windows::Forms::TextBox());
			this->m_lb_objectSelector = (gcnew System::Windows::Forms::ListBox());
			this->m_cb_selectedGenerator = (gcnew System::Windows::Forms::ComboBox());
			this->m_lbl_generatorType = (gcnew System::Windows::Forms::Label());
			this->m_pb_progress = (gcnew System::Windows::Forms::ProgressBar());
			this->m_lbl_progressStatus = (gcnew System::Windows::Forms::Label());
			this->m_menuStrip = (gcnew System::Windows::Forms::MenuStrip());
			this->m_menuItem_open = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_btn_openBlueprintOutputDirectory = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_btn_openWorldOutputDirectory = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_btn_openTileOutputDirectory = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_ts_open_separator = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->m_btn_aboutProgram = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_menuItem_settings = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_btn_reloadUserObjects = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_btn_reloadDatabase = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_ts_settings_separator = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->m_btn_options = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_lbl_objSelectorStatus = (gcnew System::Windows::Forms::Label());
			this->m_progressBarUpdater = (gcnew System::Windows::Forms::Timer(this->components));
			this->m_bw_databaseLoader = (gcnew System::ComponentModel::BackgroundWorker());
			this->m_bw_objectLoader = (gcnew System::ComponentModel::BackgroundWorker());
			this->m_bw_objectConverter = (gcnew System::ComponentModel::BackgroundWorker());
			this->m_cms_blueprint = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->m_btn_openBlueprintInSteamWorkshop = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_btn_openBlueprintFolder = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_cms_toolStrip = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->m_btn_showBlueprintInfo = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_cms_tile = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->m_btn_openTileInSteamWorkshop = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_btn_openTileFolder = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_cms_tile_separator = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->m_btn_findTileCreatorInSteam = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_cms_tile_separator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->m_btn_showTileInfo = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_btn_searchFilter = (gcnew System::Windows::Forms::Button());
			this->m_toolTip = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->m_cms_world = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->m_btn_openWorldInSteamWorkshop = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_btn_openWorldFolder = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->m_btn_showWorldInfo = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_menuStrip->SuspendLayout();
			this->m_cms_blueprint->SuspendLayout();
			this->m_cms_tile->SuspendLayout();
			this->m_cms_world->SuspendLayout();
			this->SuspendLayout();
			// 
			// m_btn_folderDialog
			// 
			this->m_btn_folderDialog->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->m_btn_folderDialog->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_btn_folderDialog->Location = System::Drawing::Point(408, 27);
			this->m_btn_folderDialog->Name = L"m_btn_folderDialog";
			this->m_btn_folderDialog->Size = System::Drawing::Size(64, 22);
			this->m_btn_folderDialog->TabIndex = 0;
			this->m_btn_folderDialog->Text = L"...";
			this->m_toolTip->SetToolTip(this->m_btn_folderDialog, L"Pick a file");
			this->m_btn_folderDialog->UseVisualStyleBackColor = true;
			this->m_btn_folderDialog->Click += gcnew System::EventHandler(this, &MainGui::FolderDialog_Click);
			// 
			// m_btn_convert
			// 
			this->m_btn_convert->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->m_btn_convert->Enabled = false;
			this->m_btn_convert->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_btn_convert->Location = System::Drawing::Point(382, 263);
			this->m_btn_convert->Name = L"m_btn_convert";
			this->m_btn_convert->Size = System::Drawing::Size(90, 36);
			this->m_btn_convert->TabIndex = 1;
			this->m_btn_convert->Text = L"Convert";
			this->m_toolTip->SetToolTip(this->m_btn_convert, L"Convert selected object");
			this->m_btn_convert->UseVisualStyleBackColor = true;
			this->m_btn_convert->Click += gcnew System::EventHandler(this, &MainGui::MainGui_Convert_Clicked);
			// 
			// m_tb_path
			// 
			this->m_tb_path->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_tb_path->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_tb_path->Location = System::Drawing::Point(12, 27);
			this->m_tb_path->MaxLength = 256;
			this->m_tb_path->Name = L"m_tb_path";
			this->m_tb_path->Size = System::Drawing::Size(390, 22);
			this->m_tb_path->TabIndex = 2;
			this->m_tb_path->TextChanged += gcnew System::EventHandler(this, &MainGui::PathTextBox_TextChanged);
			// 
			// m_tb_searchBox
			// 
			this->m_tb_searchBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_tb_searchBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_tb_searchBox->Location = System::Drawing::Point(12, 55);
			this->m_tb_searchBox->MaxLength = 64;
			this->m_tb_searchBox->Name = L"m_tb_searchBox";
			this->m_tb_searchBox->Size = System::Drawing::Size(390, 22);
			this->m_tb_searchBox->TabIndex = 3;
			this->m_tb_searchBox->TextChanged += gcnew System::EventHandler(this, &MainGui::MainGui_SearchBox_TextChanged);
			// 
			// m_lb_objectSelector
			// 
			this->m_lb_objectSelector->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_lb_objectSelector->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lb_objectSelector->FormattingEnabled = true;
			this->m_lb_objectSelector->IntegralHeight = false;
			this->m_lb_objectSelector->ItemHeight = 16;
			this->m_lb_objectSelector->Location = System::Drawing::Point(12, 83);
			this->m_lb_objectSelector->Name = L"m_lb_objectSelector";
			this->m_lb_objectSelector->Size = System::Drawing::Size(460, 128);
			this->m_lb_objectSelector->TabIndex = 4;
			this->m_lb_objectSelector->SelectedIndexChanged += gcnew System::EventHandler(this, &MainGui::MainGui_ObjectSelector_SelectedIndexChanged);
			this->m_lb_objectSelector->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MainGui::MainGui_ObjectSelector_MouseDown);
			// 
			// m_cb_selectedGenerator
			// 
			this->m_cb_selectedGenerator->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_cb_selectedGenerator->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->m_cb_selectedGenerator->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_selectedGenerator->FormattingEnabled = true;
			this->m_cb_selectedGenerator->Items->AddRange(gcnew cli::array< System::Object^  >(4) {
				L"Blueprint Converter (.json, .blueprint)",
					L"Tile Converter (.tile)", L"World Script Converter (.lua)", L"Character Converter"
			});
			this->m_cb_selectedGenerator->Location = System::Drawing::Point(12, 233);
			this->m_cb_selectedGenerator->Name = L"m_cb_selectedGenerator";
			this->m_cb_selectedGenerator->Size = System::Drawing::Size(460, 24);
			this->m_cb_selectedGenerator->TabIndex = 5;
			this->m_toolTip->SetToolTip(this->m_cb_selectedGenerator, L"Selected converter type");
			this->m_cb_selectedGenerator->SelectedIndexChanged += gcnew System::EventHandler(this, &MainGui::SelectedGenerator_SelectedIndexChanged);
			// 
			// m_lbl_generatorType
			// 
			this->m_lbl_generatorType->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->m_lbl_generatorType->AutoSize = true;
			this->m_lbl_generatorType->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lbl_generatorType->Location = System::Drawing::Point(9, 214);
			this->m_lbl_generatorType->Name = L"m_lbl_generatorType";
			this->m_lbl_generatorType->Size = System::Drawing::Size(100, 16);
			this->m_lbl_generatorType->TabIndex = 6;
			this->m_lbl_generatorType->Text = L"Converter Type";
			// 
			// m_pb_progress
			// 
			this->m_pb_progress->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_pb_progress->Location = System::Drawing::Point(12, 284);
			this->m_pb_progress->Name = L"m_pb_progress";
			this->m_pb_progress->Size = System::Drawing::Size(364, 15);
			this->m_pb_progress->TabIndex = 7;
			// 
			// m_lbl_progressStatus
			// 
			this->m_lbl_progressStatus->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->m_lbl_progressStatus->AutoSize = true;
			this->m_lbl_progressStatus->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lbl_progressStatus->Location = System::Drawing::Point(9, 265);
			this->m_lbl_progressStatus->Name = L"m_lbl_progressStatus";
			this->m_lbl_progressStatus->Size = System::Drawing::Size(104, 16);
			this->m_lbl_progressStatus->TabIndex = 9;
			this->m_lbl_progressStatus->Text = L"STATUS_TEXT";
			// 
			// m_menuStrip
			// 
			this->m_menuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
				this->m_menuItem_open,
					this->m_menuItem_settings
			});
			this->m_menuStrip->Location = System::Drawing::Point(0, 0);
			this->m_menuStrip->Name = L"m_menuStrip";
			this->m_menuStrip->RenderMode = System::Windows::Forms::ToolStripRenderMode::System;
			this->m_menuStrip->Size = System::Drawing::Size(484, 24);
			this->m_menuStrip->TabIndex = 10;
			this->m_menuStrip->Text = L"Menu Strip";
			// 
			// m_menuItem_open
			// 
			this->m_menuItem_open->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {
				this->m_btn_openBlueprintOutputDirectory,
					this->m_btn_openWorldOutputDirectory, this->m_btn_openTileOutputDirectory, this->m_ts_open_separator, this->m_btn_aboutProgram
			});
			this->m_menuItem_open->Name = L"m_menuItem_open";
			this->m_menuItem_open->Size = System::Drawing::Size(48, 20);
			this->m_menuItem_open->Text = L"Open";
			// 
			// m_btn_openBlueprintOutputDirectory
			// 
			this->m_btn_openBlueprintOutputDirectory->Name = L"m_btn_openBlueprintOutputDirectory";
			this->m_btn_openBlueprintOutputDirectory->Size = System::Drawing::Size(214, 22);
			this->m_btn_openBlueprintOutputDirectory->Text = L"Blueprint Output Directory";
			this->m_btn_openBlueprintOutputDirectory->Click += gcnew System::EventHandler(this, &MainGui::MainGui_OpenBlueprintOutputFolder_Click);
			// 
			// m_btn_openWorldOutputDirectory
			// 
			this->m_btn_openWorldOutputDirectory->Name = L"m_btn_openWorldOutputDirectory";
			this->m_btn_openWorldOutputDirectory->Size = System::Drawing::Size(214, 22);
			this->m_btn_openWorldOutputDirectory->Text = L"World Output Directory";
			this->m_btn_openWorldOutputDirectory->Click += gcnew System::EventHandler(this, &MainGui::MainGui_OpenWorldOutputDirectory_Click);
			// 
			// m_btn_openTileOutputDirectory
			// 
			this->m_btn_openTileOutputDirectory->Name = L"m_btn_openTileOutputDirectory";
			this->m_btn_openTileOutputDirectory->Size = System::Drawing::Size(214, 22);
			this->m_btn_openTileOutputDirectory->Text = L"Tile Output Directory";
			this->m_btn_openTileOutputDirectory->Click += gcnew System::EventHandler(this, &MainGui::MainGui_OpenTileOutputFolder_Click);
			// 
			// m_ts_open_separator
			// 
			this->m_ts_open_separator->Name = L"m_ts_open_separator";
			this->m_ts_open_separator->Size = System::Drawing::Size(211, 6);
			// 
			// m_btn_aboutProgram
			// 
			this->m_btn_aboutProgram->Name = L"m_btn_aboutProgram";
			this->m_btn_aboutProgram->Size = System::Drawing::Size(214, 22);
			this->m_btn_aboutProgram->Text = L"About Program";
			this->m_btn_aboutProgram->Click += gcnew System::EventHandler(this, &MainGui::AboutButton_Click);
			// 
			// m_menuItem_settings
			// 
			this->m_menuItem_settings->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {
				this->m_btn_reloadUserObjects,
					this->m_btn_reloadDatabase, this->m_ts_settings_separator, this->m_btn_options
			});
			this->m_menuItem_settings->Name = L"m_menuItem_settings";
			this->m_menuItem_settings->Size = System::Drawing::Size(61, 20);
			this->m_menuItem_settings->Text = L"Settings";
			// 
			// m_btn_reloadUserObjects
			// 
			this->m_btn_reloadUserObjects->Name = L"m_btn_reloadUserObjects";
			this->m_btn_reloadUserObjects->Size = System::Drawing::Size(199, 22);
			this->m_btn_reloadUserObjects->Text = L"Reload User Objects";
			this->m_btn_reloadUserObjects->Click += gcnew System::EventHandler(this, &MainGui::MainGui_ReloadUserObjects_Click);
			// 
			// m_btn_reloadDatabase
			// 
			this->m_btn_reloadDatabase->Name = L"m_btn_reloadDatabase";
			this->m_btn_reloadDatabase->Size = System::Drawing::Size(199, 22);
			this->m_btn_reloadDatabase->Text = L"Reload Object Database";
			this->m_btn_reloadDatabase->Click += gcnew System::EventHandler(this, &MainGui::MainGui_ReloadDatabase_Click);
			// 
			// m_ts_settings_separator
			// 
			this->m_ts_settings_separator->Name = L"m_ts_settings_separator";
			this->m_ts_settings_separator->Size = System::Drawing::Size(196, 6);
			// 
			// m_btn_options
			// 
			this->m_btn_options->Name = L"m_btn_options";
			this->m_btn_options->Size = System::Drawing::Size(199, 22);
			this->m_btn_options->Text = L"Options";
			this->m_btn_options->Click += gcnew System::EventHandler(this, &MainGui::MainGui_Options_Click);
			// 
			// m_lbl_objSelectorStatus
			// 
			this->m_lbl_objSelectorStatus->BackColor = System::Drawing::SystemColors::ControlLightLight;
			this->m_lbl_objSelectorStatus->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lbl_objSelectorStatus->Location = System::Drawing::Point(114, 137);
			this->m_lbl_objSelectorStatus->Name = L"m_lbl_objSelectorStatus";
			this->m_lbl_objSelectorStatus->Size = System::Drawing::Size(259, 20);
			this->m_lbl_objSelectorStatus->TabIndex = 0;
			this->m_lbl_objSelectorStatus->Text = L"OBJECT_SELECTOR_STATUS";
			this->m_lbl_objSelectorStatus->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// m_progressBarUpdater
			// 
			this->m_progressBarUpdater->Interval = 50;
			this->m_progressBarUpdater->Tick += gcnew System::EventHandler(this, &MainGui::MainGui_Timer_UpdateProgressBar);
			// 
			// m_bw_databaseLoader
			// 
			this->m_bw_databaseLoader->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MainGui::MainGui_DatabaseLoader_DoWork);
			this->m_bw_databaseLoader->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &MainGui::MainGui_DatabaseLoader_RunWorkerCompleted);
			// 
			// m_bw_objectLoader
			// 
			this->m_bw_objectLoader->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MainGui::MainGui_ObjectLoader_DoWork);
			this->m_bw_objectLoader->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &MainGui::MainGui_ObjectLoader_RunWorkerCompleted);
			// 
			// m_bw_objectConverter
			// 
			this->m_bw_objectConverter->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MainGui::MainGui_ObjectConverter_DoWork);
			this->m_bw_objectConverter->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &MainGui::MainGui_ObjectConverter_RunWorkerCompleted);
			// 
			// m_cms_blueprint
			// 
			this->m_cms_blueprint->ImeMode = System::Windows::Forms::ImeMode::NoControl;
			this->m_cms_blueprint->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {
				this->m_btn_openBlueprintInSteamWorkshop,
					this->m_btn_openBlueprintFolder, this->m_cms_toolStrip, this->m_btn_showBlueprintInfo
			});
			this->m_cms_blueprint->Name = L"m_cms_blueprint";
			this->m_cms_blueprint->RenderMode = System::Windows::Forms::ToolStripRenderMode::System;
			this->m_cms_blueprint->Size = System::Drawing::Size(210, 76);
			// 
			// m_btn_openBlueprintInSteamWorkshop
			// 
			this->m_btn_openBlueprintInSteamWorkshop->Name = L"m_btn_openBlueprintInSteamWorkshop";
			this->m_btn_openBlueprintInSteamWorkshop->Size = System::Drawing::Size(209, 22);
			this->m_btn_openBlueprintInSteamWorkshop->Text = L"Open in Steam Workshop";
			this->m_btn_openBlueprintInSteamWorkshop->Click += gcnew System::EventHandler(this, &MainGui::MainGui_OpenItemInWorkshop);
			// 
			// m_btn_openBlueprintFolder
			// 
			this->m_btn_openBlueprintFolder->Name = L"m_btn_openBlueprintFolder";
			this->m_btn_openBlueprintFolder->Size = System::Drawing::Size(209, 22);
			this->m_btn_openBlueprintFolder->Text = L"Open in Explorer";
			this->m_btn_openBlueprintFolder->Click += gcnew System::EventHandler(this, &MainGui::MainGui_OpenItemDirectory);
			// 
			// m_cms_toolStrip
			// 
			this->m_cms_toolStrip->Name = L"m_cms_toolStrip";
			this->m_cms_toolStrip->Size = System::Drawing::Size(206, 6);
			// 
			// m_btn_showBlueprintInfo
			// 
			this->m_btn_showBlueprintInfo->Name = L"m_btn_showBlueprintInfo";
			this->m_btn_showBlueprintInfo->Size = System::Drawing::Size(209, 22);
			this->m_btn_showBlueprintInfo->Text = L"Show Blueprint Info";
			this->m_btn_showBlueprintInfo->Click += gcnew System::EventHandler(this, &MainGui::MainGui_ShowBlueprintInfo_Click);
			// 
			// m_cms_tile
			// 
			this->m_cms_tile->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {
				this->m_btn_openTileInSteamWorkshop,
					this->m_btn_openTileFolder, this->m_cms_tile_separator, this->m_btn_findTileCreatorInSteam, this->m_cms_tile_separator2, this->m_btn_showTileInfo
			});
			this->m_cms_tile->Name = L"m_cms_tile";
			this->m_cms_tile->RenderMode = System::Windows::Forms::ToolStripRenderMode::System;
			this->m_cms_tile->Size = System::Drawing::Size(210, 104);
			// 
			// m_btn_openTileInSteamWorkshop
			// 
			this->m_btn_openTileInSteamWorkshop->Name = L"m_btn_openTileInSteamWorkshop";
			this->m_btn_openTileInSteamWorkshop->Size = System::Drawing::Size(209, 22);
			this->m_btn_openTileInSteamWorkshop->Text = L"Open in Steam Workshop";
			this->m_btn_openTileInSteamWorkshop->Click += gcnew System::EventHandler(this, &MainGui::MainGui_OpenItemInWorkshop);
			// 
			// m_btn_openTileFolder
			// 
			this->m_btn_openTileFolder->Name = L"m_btn_openTileFolder";
			this->m_btn_openTileFolder->Size = System::Drawing::Size(209, 22);
			this->m_btn_openTileFolder->Text = L"Open in Explorer";
			this->m_btn_openTileFolder->Click += gcnew System::EventHandler(this, &MainGui::MainGui_OpenItemDirectory);
			// 
			// m_cms_tile_separator
			// 
			this->m_cms_tile_separator->Name = L"m_cms_tile_separator";
			this->m_cms_tile_separator->Size = System::Drawing::Size(206, 6);
			// 
			// m_btn_findTileCreatorInSteam
			// 
			this->m_btn_findTileCreatorInSteam->Name = L"m_btn_findTileCreatorInSteam";
			this->m_btn_findTileCreatorInSteam->Size = System::Drawing::Size(209, 22);
			this->m_btn_findTileCreatorInSteam->Text = L"Find Creator in Steam";
			this->m_btn_findTileCreatorInSteam->Click += gcnew System::EventHandler(this, &MainGui::MainGui_FindTileCreator_Click);
			// 
			// m_cms_tile_separator2
			// 
			this->m_cms_tile_separator2->Name = L"m_cms_tile_separator2";
			this->m_cms_tile_separator2->Size = System::Drawing::Size(206, 6);
			// 
			// m_btn_showTileInfo
			// 
			this->m_btn_showTileInfo->Name = L"m_btn_showTileInfo";
			this->m_btn_showTileInfo->Size = System::Drawing::Size(209, 22);
			this->m_btn_showTileInfo->Text = L"Show Tile Info";
			this->m_btn_showTileInfo->Click += gcnew System::EventHandler(this, &MainGui::MainGui_ShowTileInfo_Click);
			// 
			// m_btn_searchFilter
			// 
			this->m_btn_searchFilter->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->m_btn_searchFilter->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_btn_searchFilter->Location = System::Drawing::Point(408, 55);
			this->m_btn_searchFilter->Name = L"m_btn_searchFilter";
			this->m_btn_searchFilter->Size = System::Drawing::Size(64, 22);
			this->m_btn_searchFilter->TabIndex = 11;
			this->m_btn_searchFilter->Text = L"Filter";
			this->m_toolTip->SetToolTip(this->m_btn_searchFilter, L"Object Filter");
			this->m_btn_searchFilter->UseVisualStyleBackColor = true;
			this->m_btn_searchFilter->Click += gcnew System::EventHandler(this, &MainGui::MainGui_Filter_Click);
			// 
			// m_cms_world
			// 
			this->m_cms_world->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {
				this->m_btn_openWorldInSteamWorkshop,
					this->m_btn_openWorldFolder, this->toolStripSeparator1, this->m_btn_showWorldInfo
			});
			this->m_cms_world->Name = L"m_cms_world";
			this->m_cms_world->RenderMode = System::Windows::Forms::ToolStripRenderMode::System;
			this->m_cms_world->Size = System::Drawing::Size(210, 76);
			// 
			// m_btn_openWorldInSteamWorkshop
			// 
			this->m_btn_openWorldInSteamWorkshop->Name = L"m_btn_openWorldInSteamWorkshop";
			this->m_btn_openWorldInSteamWorkshop->Size = System::Drawing::Size(209, 22);
			this->m_btn_openWorldInSteamWorkshop->Text = L"Open in Steam Workshop";
			this->m_btn_openWorldInSteamWorkshop->Click += gcnew System::EventHandler(this, &MainGui::MainGui_OpenItemInWorkshop);
			// 
			// m_btn_openWorldFolder
			// 
			this->m_btn_openWorldFolder->Name = L"m_btn_openWorldFolder";
			this->m_btn_openWorldFolder->Size = System::Drawing::Size(209, 22);
			this->m_btn_openWorldFolder->Text = L"Open in Explorer";
			this->m_btn_openWorldFolder->Click += gcnew System::EventHandler(this, &MainGui::MainGui_OpenItemDirectory);
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(206, 6);
			// 
			// m_btn_showWorldInfo
			// 
			this->m_btn_showWorldInfo->Name = L"m_btn_showWorldInfo";
			this->m_btn_showWorldInfo->Size = System::Drawing::Size(209, 22);
			this->m_btn_showWorldInfo->Text = L"Show World Info";
			this->m_btn_showWorldInfo->Click += gcnew System::EventHandler(this, &MainGui::MainGui_ShowWorldInfo_Click);
			// 
			// MainGui
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(484, 311);
			this->Controls->Add(this->m_btn_searchFilter);
			this->Controls->Add(this->m_lbl_objSelectorStatus);
			this->Controls->Add(this->m_lbl_progressStatus);
			this->Controls->Add(this->m_pb_progress);
			this->Controls->Add(this->m_lbl_generatorType);
			this->Controls->Add(this->m_cb_selectedGenerator);
			this->Controls->Add(this->m_lb_objectSelector);
			this->Controls->Add(this->m_tb_searchBox);
			this->Controls->Add(this->m_tb_path);
			this->Controls->Add(this->m_btn_convert);
			this->Controls->Add(this->m_btn_folderDialog);
			this->Controls->Add(this->m_menuStrip);
			this->MainMenuStrip = this->m_menuStrip;
			this->MinimumSize = System::Drawing::Size(500, 350);
			this->Name = L"MainGui";
			this->ShowIcon = false;
			this->Text = L"Scrap Mechanic Converter";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MainGui::MainGui_FormClosing);
			this->Shown += gcnew System::EventHandler(this, &MainGui::MainGui_Shown);
			this->Resize += gcnew System::EventHandler(this, &MainGui::MainGui_Resize);
			this->m_menuStrip->ResumeLayout(false);
			this->m_menuStrip->PerformLayout();
			this->m_cms_blueprint->ResumeLayout(false);
			this->m_cms_tile->ResumeLayout(false);
			this->m_cms_world->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		System::Void MainGui_Shown(System::Object^ sender, System::EventArgs^ e);
		System::Void MainGui_UpdatePathTextBox();
		System::Void MainGui_UpdatePathListContextMenuStrip();
		System::Void SelectedGenerator_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void PathTextBox_TextChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void FolderDialog_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void AboutButton_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void MainGui_Resize(System::Object^ sender, System::EventArgs^ e);
		System::Void MainGui_Timer_UpdateProgressBar(System::Object^ sender, System::EventArgs^ e);
		System::Void LoadObjectDatabase(const bool& should_reload);
		System::Void MainGui_DatabaseLoader_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e);
		System::Void MainGui_DatabaseLoader_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e);
		System::Void MainGui_ChangeGuiState(bool db_loaded, bool objlist_loaded, bool obj_converted);
		System::Void MainGui_ReloadDatabase_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void LoadUserObjects();
		System::Void UpdateObjectListStatus();
		System::Void UpdateCurrentObjectList();
		System::Void MainGui_ObjectLoader_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e);
		System::Void MainGui_ObjectLoader_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e);
		System::Void MainGui_ReloadUserObjects_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void MainGui_ObjectSelector_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
		System::Void MainGui_ObjectSelector_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void UpdateSearchResults(int last_search_length);
		System::Void MainGui_SearchBox_TextChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void UpdateConvertButton();
		System::Void ResetProgressBar();

		System::Void MainGui_ConvertBlueprint(const std::wstring& filename, const std::wstring& path);
		System::Void MainGui_ConvertTile(const std::wstring& filename, const std::wstring& path);
		System::Void MainGui_ConvertWorld(const std::wstring& filename, const std::wstring& path);
		System::Void MainGui_Convert_Clicked(System::Object^ sender, System::EventArgs^ e);

		std::vector<BlueprintInstance*>& GetCurrentBlueprintList();
		BlueprintInstance* GetCurrentBlueprint();

		std::vector<TileInstance*>& GetCurrentTileList();
		TileInstance* GetCurrentTile();

		std::vector<WorldInstance*>& GetCurrentWorldList();
		WorldInstance* GetCurrentWorld();

		System::Void MainGui_HandleConvertError(ConvertError& v_error, const int& v_type, System::ComponentModel::DoWorkEventArgs^ e);
		System::Void ObjectConverter_ConvertBlueprint(System::Array^ conv_data, System::ComponentModel::DoWorkEventArgs^ e);
		System::Void ObjectConverter_ConvertTile(System::Array^ conv_data, System::ComponentModel::DoWorkEventArgs^ e);
		System::Void ObjectConverter_ConvertWorld(System::Array^ conv_data, System::ComponentModel::DoWorkEventArgs^ e);

		System::Void MainGui_ObjectConverter_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e);
		System::Void MainGui_ObjectConverter_RunWorkerCompleted(System::Object^ sender, System::ComponentModel::RunWorkerCompletedEventArgs^ e);
		System::Void MainGui_Options_Click(System::Object^ sender, System::EventArgs^ e);

		System::Void MainGui_OpenItemInWorkshop(System::Object^ sender, System::EventArgs^ e);
		System::Void MainGui_OpenItemDirectory(System::Object^ sender, System::EventArgs^ e);

		System::Void UpdateContextMenuStrip();
		System::Void MainGui_FindTileCreator_Click(System::Object^ sender, System::EventArgs^ e);

		System::Void MainGui_OpenBlueprintOutputFolder_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void MainGui_OpenWorldOutputDirectory_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void MainGui_OpenTileOutputFolder_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void MainGui_Filter_Click(System::Object^ sender, System::EventArgs^ e);

		System::Void MainGui_ShowBlueprintInfo_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void MainGui_ShowTileInfo_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void MainGui_ShowWorldInfo_Click(System::Object^ sender, System::EventArgs^ e);

		System::Void MainGui_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e);

		System::Void MainGui_CenterChildForm(System::Windows::Forms::Form^ form);
	};
}
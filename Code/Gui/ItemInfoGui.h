#pragma once

#include "ObjectDatabase\UserDataReaders\BlueprintFolderReader.hpp"
#include "ObjectDatabase\UserDataReaders\TileFolderReader.hpp"

namespace SMConverter 
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class ItemInfoGui : public System::Windows::Forms::Form
	{
	public:
		ItemInfoGui(BlueprintInstance* v_blueprint);
		ItemInfoGui(TileInstance* v_tile);

	protected:
		~ItemInfoGui();

	private:
		System::Windows::Forms::PictureBox^ m_bp_blueprintPreview;
		System::Windows::Forms::ListBox^ m_lb_modSelector;
		System::Windows::Forms::Label^ m_lbl_modList;
		System::Windows::Forms::ContextMenuStrip^ m_cms_modOptions;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_openInSteamWorkshop;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_openInExplorer;
		System::Windows::Forms::TextBox^ m_tb_itemInfo;

		System::ComponentModel::IContainer^ components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->m_bp_blueprintPreview = (gcnew System::Windows::Forms::PictureBox());
			this->m_lb_modSelector = (gcnew System::Windows::Forms::ListBox());
			this->m_cms_modOptions = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->m_btn_openInSteamWorkshop = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_btn_openInExplorer = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_lbl_modList = (gcnew System::Windows::Forms::Label());
			this->m_tb_itemInfo = (gcnew System::Windows::Forms::TextBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_bp_blueprintPreview))->BeginInit();
			this->m_cms_modOptions->SuspendLayout();
			this->SuspendLayout();
			// 
			// m_bp_blueprintPreview
			// 
			this->m_bp_blueprintPreview->BackColor = System::Drawing::SystemColors::ControlLightLight;
			this->m_bp_blueprintPreview->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->m_bp_blueprintPreview->Location = System::Drawing::Point(12, 12);
			this->m_bp_blueprintPreview->Name = L"m_bp_blueprintPreview";
			this->m_bp_blueprintPreview->Size = System::Drawing::Size(100, 100);
			this->m_bp_blueprintPreview->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->m_bp_blueprintPreview->TabIndex = 0;
			this->m_bp_blueprintPreview->TabStop = false;
			// 
			// m_lb_modSelector
			// 
			this->m_lb_modSelector->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_lb_modSelector->ContextMenuStrip = this->m_cms_modOptions;
			this->m_lb_modSelector->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lb_modSelector->FormattingEnabled = true;
			this->m_lb_modSelector->IntegralHeight = false;
			this->m_lb_modSelector->ItemHeight = 16;
			this->m_lb_modSelector->Location = System::Drawing::Point(12, 138);
			this->m_lb_modSelector->Name = L"m_lb_modSelector";
			this->m_lb_modSelector->Size = System::Drawing::Size(410, 120);
			this->m_lb_modSelector->TabIndex = 4;
			this->m_lb_modSelector->SelectedIndexChanged += gcnew System::EventHandler(this, &ItemInfoGui::ModList_SelectedIndexChanged);
			this->m_lb_modSelector->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &ItemInfoGui::ModSelector_MouseDown);
			// 
			// m_cms_modOptions
			// 
			this->m_cms_modOptions->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
				this->m_btn_openInSteamWorkshop,
					this->m_btn_openInExplorer
			});
			this->m_cms_modOptions->Name = L"m_cms_modOptions";
			this->m_cms_modOptions->RenderMode = System::Windows::Forms::ToolStripRenderMode::System;
			this->m_cms_modOptions->Size = System::Drawing::Size(210, 48);
			// 
			// m_btn_openInSteamWorkshop
			// 
			this->m_btn_openInSteamWorkshop->Enabled = false;
			this->m_btn_openInSteamWorkshop->Name = L"m_btn_openInSteamWorkshop";
			this->m_btn_openInSteamWorkshop->Size = System::Drawing::Size(209, 22);
			this->m_btn_openInSteamWorkshop->Text = L"Open in Steam Workshop";
			this->m_btn_openInSteamWorkshop->Click += gcnew System::EventHandler(this, &ItemInfoGui::ModList_OpenInSteamWorkshop_Click);
			// 
			// m_btn_openInExplorer
			// 
			this->m_btn_openInExplorer->Enabled = false;
			this->m_btn_openInExplorer->Name = L"m_btn_openInExplorer";
			this->m_btn_openInExplorer->Size = System::Drawing::Size(209, 22);
			this->m_btn_openInExplorer->Text = L"Open in Explorer";
			this->m_btn_openInExplorer->Click += gcnew System::EventHandler(this, &ItemInfoGui::ModList_OpenInExplorer_Click);
			// 
			// m_lbl_modList
			// 
			this->m_lbl_modList->AutoSize = true;
			this->m_lbl_modList->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lbl_modList->Location = System::Drawing::Point(9, 119);
			this->m_lbl_modList->Name = L"m_lbl_modList";
			this->m_lbl_modList->Size = System::Drawing::Size(57, 16);
			this->m_lbl_modList->TabIndex = 5;
			this->m_lbl_modList->Text = L"Mod List";
			// 
			// m_tb_itemInfo
			// 
			this->m_tb_itemInfo->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_tb_itemInfo->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->m_tb_itemInfo->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_tb_itemInfo->ImeMode = System::Windows::Forms::ImeMode::Disable;
			this->m_tb_itemInfo->Location = System::Drawing::Point(118, 12);
			this->m_tb_itemInfo->Multiline = true;
			this->m_tb_itemInfo->Name = L"m_tb_itemInfo";
			this->m_tb_itemInfo->ReadOnly = true;
			this->m_tb_itemInfo->ShortcutsEnabled = false;
			this->m_tb_itemInfo->Size = System::Drawing::Size(304, 100);
			this->m_tb_itemInfo->TabIndex = 6;
			// 
			// ItemInfoGui
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(434, 270);
			this->Controls->Add(this->m_tb_itemInfo);
			this->Controls->Add(this->m_lbl_modList);
			this->Controls->Add(this->m_lb_modSelector);
			this->Controls->Add(this->m_bp_blueprintPreview);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"ItemInfoGui";
			this->ShowIcon = false;
			this->Text = L"ITEM_INFO";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->m_bp_blueprintPreview))->EndInit();
			this->m_cms_modOptions->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

		System::Void UpdateModList();
		SMMod* GetCurrentMod();

		System::Void ModSelector_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
		System::Void ModList_OpenInSteamWorkshop_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void ModList_OpenInExplorer_Click(System::Object^ sender, System::EventArgs^ e);
		System::Void ModList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
	};
}

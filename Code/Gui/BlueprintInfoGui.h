#pragma once

#include "ObjectDatabase\UserDataReaders\BlueprintFolderReader.hpp"

namespace SMConverter
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class BlueprintInfoGui : public System::Windows::Forms::Form
	{
	public:
		BlueprintInfoGui(BlueprintInstance* v_blueprint);
		BlueprintInstance* m_blueprint;

	protected:
		~BlueprintInfoGui();
		
	private:
		System::Windows::Forms::PictureBox^ m_bp_blueprintPreview;
		System::Windows::Forms::Label^ m_lbl_blueprintName;
		System::Windows::Forms::Label^ m_lbl_blueprintUuid;
		System::Windows::Forms::Label^ m_lbl_contentType;
		System::Windows::Forms::ListBox^ m_lb_modSelector;
		System::Windows::Forms::Label^ label1;
		System::Windows::Forms::Label^ m_lbl_partCount;
		System::Windows::Forms::Label^ m_lbl_modCount;
		System::Windows::Forms::ContextMenuStrip^ m_cms_modOptions;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_openInSteamWorkshop;
		System::Windows::Forms::ToolStripMenuItem^ m_btn_openInExplorer;

		System::ComponentModel::IContainer^ components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->m_bp_blueprintPreview = (gcnew System::Windows::Forms::PictureBox());
			this->m_lbl_blueprintName = (gcnew System::Windows::Forms::Label());
			this->m_lbl_blueprintUuid = (gcnew System::Windows::Forms::Label());
			this->m_lbl_contentType = (gcnew System::Windows::Forms::Label());
			this->m_lb_modSelector = (gcnew System::Windows::Forms::ListBox());
			this->m_cms_modOptions = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->m_btn_openInSteamWorkshop = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_btn_openInExplorer = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->m_lbl_partCount = (gcnew System::Windows::Forms::Label());
			this->m_lbl_modCount = (gcnew System::Windows::Forms::Label());
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
			// m_lbl_blueprintName
			// 
			this->m_lbl_blueprintName->AutoSize = true;
			this->m_lbl_blueprintName->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lbl_blueprintName->Location = System::Drawing::Point(118, 12);
			this->m_lbl_blueprintName->Name = L"m_lbl_blueprintName";
			this->m_lbl_blueprintName->Size = System::Drawing::Size(50, 16);
			this->m_lbl_blueprintName->TabIndex = 1;
			this->m_lbl_blueprintName->Text = L"Name: ";
			// 
			// m_lbl_blueprintUuid
			// 
			this->m_lbl_blueprintUuid->AutoSize = true;
			this->m_lbl_blueprintUuid->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lbl_blueprintUuid->Location = System::Drawing::Point(118, 28);
			this->m_lbl_blueprintUuid->Name = L"m_lbl_blueprintUuid";
			this->m_lbl_blueprintUuid->Size = System::Drawing::Size(41, 16);
			this->m_lbl_blueprintUuid->TabIndex = 2;
			this->m_lbl_blueprintUuid->Text = L"Uuid: ";
			// 
			// m_lbl_contentType
			// 
			this->m_lbl_contentType->AutoSize = true;
			this->m_lbl_contentType->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lbl_contentType->Location = System::Drawing::Point(118, 44);
			this->m_lbl_contentType->Name = L"m_lbl_contentType";
			this->m_lbl_contentType->Size = System::Drawing::Size(93, 16);
			this->m_lbl_contentType->TabIndex = 3;
			this->m_lbl_contentType->Text = L"Content Type: ";
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
			this->m_lb_modSelector->SelectedIndexChanged += gcnew System::EventHandler(this, &BlueprintInfoGui::ModList_SelectedIndexChanged);
			this->m_lb_modSelector->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &BlueprintInfoGui::ModSelector_MouseDown);
			// 
			// m_cms_modOptions
			// 
			this->m_cms_modOptions->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
				this->m_btn_openInSteamWorkshop,
					this->m_btn_openInExplorer
			});
			this->m_cms_modOptions->Name = L"m_cms_modOptions";
			this->m_cms_modOptions->RenderMode = System::Windows::Forms::ToolStripRenderMode::System;
			this->m_cms_modOptions->Size = System::Drawing::Size(210, 70);
			// 
			// m_btn_openInSteamWorkshop
			// 
			this->m_btn_openInSteamWorkshop->Enabled = false;
			this->m_btn_openInSteamWorkshop->Name = L"m_btn_openInSteamWorkshop";
			this->m_btn_openInSteamWorkshop->Size = System::Drawing::Size(209, 22);
			this->m_btn_openInSteamWorkshop->Text = L"Open in Steam Workshop";
			this->m_btn_openInSteamWorkshop->Click += gcnew System::EventHandler(this, &BlueprintInfoGui::ModList_OpenInSteamWorkshop_Click);
			// 
			// m_btn_openInExplorer
			// 
			this->m_btn_openInExplorer->Enabled = false;
			this->m_btn_openInExplorer->Name = L"m_btn_openInExplorer";
			this->m_btn_openInExplorer->Size = System::Drawing::Size(209, 22);
			this->m_btn_openInExplorer->Text = L"Open in Explorer";
			this->m_btn_openInExplorer->Click += gcnew System::EventHandler(this, &BlueprintInfoGui::ModList_OpenInExplorer_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->label1->Location = System::Drawing::Point(9, 119);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(57, 16);
			this->label1->TabIndex = 5;
			this->label1->Text = L"Mod List";
			// 
			// m_lbl_partCount
			// 
			this->m_lbl_partCount->AutoSize = true;
			this->m_lbl_partCount->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lbl_partCount->Location = System::Drawing::Point(118, 60);
			this->m_lbl_partCount->Name = L"m_lbl_partCount";
			this->m_lbl_partCount->Size = System::Drawing::Size(133, 16);
			this->m_lbl_partCount->TabIndex = 6;
			this->m_lbl_partCount->Text = L"Part Count: LOADING";
			// 
			// m_lbl_modCount
			// 
			this->m_lbl_modCount->AutoSize = true;
			this->m_lbl_modCount->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lbl_modCount->Location = System::Drawing::Point(118, 76);
			this->m_lbl_modCount->Name = L"m_lbl_modCount";
			this->m_lbl_modCount->Size = System::Drawing::Size(136, 16);
			this->m_lbl_modCount->TabIndex = 7;
			this->m_lbl_modCount->Text = L"Mod Count: LOADING";
			// 
			// BlueprintInfoGui
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(434, 270);
			this->Controls->Add(this->m_lbl_modCount);
			this->Controls->Add(this->m_lbl_partCount);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->m_lb_modSelector);
			this->Controls->Add(this->m_lbl_contentType);
			this->Controls->Add(this->m_lbl_blueprintUuid);
			this->Controls->Add(this->m_lbl_blueprintName);
			this->Controls->Add(this->m_bp_blueprintPreview);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"BlueprintInfoGui";
			this->ShowIcon = false;
			this->Text = L"Blueprint Info";
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

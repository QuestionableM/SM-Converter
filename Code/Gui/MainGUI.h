#pragma once

namespace SMConverter {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MainGui
	/// </summary>
	public ref class MainGui : public System::Windows::Forms::Form
	{
	public:
		MainGui(void);

	protected:
		~MainGui();

	private: System::Windows::Forms::Button^ m_btn_folderDialog;
	private: System::Windows::Forms::Button^ m_btn_convert;
	protected:

	protected:

	private: System::Windows::Forms::TextBox^ m_tb_path;
	private: System::Windows::Forms::TextBox^ m_tb_searchBox;
	private: System::Windows::Forms::ListBox^ m_lb_objectSelector;



	private: System::Windows::Forms::ComboBox^ m_cb_selectedGenerator;
	private: System::Windows::Forms::Label^ m_lbl_generatorType;
	private: System::Windows::Forms::ProgressBar^ m_pb_progress;
	private: System::Windows::Forms::Label^ m_lbl_progressStatus;
	private: System::Windows::Forms::MenuStrip^ m_menuStrip;
	private: System::Windows::Forms::ToolStripMenuItem^ aboutToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ settingsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ reloadObjectDatabaseToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^ toolStripSeparator1;
	private: System::Windows::Forms::ToolStripMenuItem^ optionsToolStripMenuItem;







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
			this->aboutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->settingsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->reloadObjectDatabaseToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->optionsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->m_menuStrip->SuspendLayout();
			this->SuspendLayout();
			// 
			// m_btn_folderDialog
			// 
			this->m_btn_folderDialog->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->m_btn_folderDialog->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_btn_folderDialog->Location = System::Drawing::Point(328, 27);
			this->m_btn_folderDialog->Name = L"m_btn_folderDialog";
			this->m_btn_folderDialog->Size = System::Drawing::Size(44, 22);
			this->m_btn_folderDialog->TabIndex = 0;
			this->m_btn_folderDialog->Text = L"...";
			this->m_btn_folderDialog->UseVisualStyleBackColor = true;
			// 
			// m_btn_convert
			// 
			this->m_btn_convert->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->m_btn_convert->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_btn_convert->Location = System::Drawing::Point(282, 263);
			this->m_btn_convert->Name = L"m_btn_convert";
			this->m_btn_convert->Size = System::Drawing::Size(90, 36);
			this->m_btn_convert->TabIndex = 1;
			this->m_btn_convert->Text = L"Convert";
			this->m_btn_convert->UseVisualStyleBackColor = true;
			// 
			// m_tb_path
			// 
			this->m_tb_path->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_tb_path->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_tb_path->Location = System::Drawing::Point(12, 27);
			this->m_tb_path->Name = L"m_tb_path";
			this->m_tb_path->Size = System::Drawing::Size(310, 22);
			this->m_tb_path->TabIndex = 2;
			// 
			// m_tb_searchBox
			// 
			this->m_tb_searchBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_tb_searchBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_tb_searchBox->Location = System::Drawing::Point(12, 55);
			this->m_tb_searchBox->Name = L"m_tb_searchBox";
			this->m_tb_searchBox->Size = System::Drawing::Size(360, 22);
			this->m_tb_searchBox->TabIndex = 3;
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
			this->m_lb_objectSelector->Size = System::Drawing::Size(360, 128);
			this->m_lb_objectSelector->TabIndex = 4;
			// 
			// m_cb_selectedGenerator
			// 
			this->m_cb_selectedGenerator->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_cb_selectedGenerator->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->m_cb_selectedGenerator->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_selectedGenerator->FormattingEnabled = true;
			this->m_cb_selectedGenerator->Items->AddRange(gcnew cli::array< System::Object^  >(3) {
				L"Blueprint Converter (.json, .blueprint)",
					L"Tile Converter (.tile)", L"World Script Converter (.lua)"
			});
			this->m_cb_selectedGenerator->Location = System::Drawing::Point(12, 233);
			this->m_cb_selectedGenerator->Name = L"m_cb_selectedGenerator";
			this->m_cb_selectedGenerator->Size = System::Drawing::Size(360, 24);
			this->m_cb_selectedGenerator->TabIndex = 5;
			this->m_cb_selectedGenerator->SelectedIndexChanged += gcnew System::EventHandler(this, &MainGui::SelectedGenerator_SelectedIndexChanged);
			// 
			// m_lbl_generatorType
			// 
			this->m_lbl_generatorType->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->m_lbl_generatorType->AutoSize = true;
			this->m_lbl_generatorType->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lbl_generatorType->Location = System::Drawing::Point(9, 214);
			this->m_lbl_generatorType->Name = L"m_lbl_generatorType";
			this->m_lbl_generatorType->Size = System::Drawing::Size(102, 16);
			this->m_lbl_generatorType->TabIndex = 6;
			this->m_lbl_generatorType->Text = L"Generator Type";
			// 
			// m_pb_progress
			// 
			this->m_pb_progress->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_pb_progress->Location = System::Drawing::Point(12, 284);
			this->m_pb_progress->Name = L"m_pb_progress";
			this->m_pb_progress->Size = System::Drawing::Size(264, 15);
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
				this->aboutToolStripMenuItem,
					this->settingsToolStripMenuItem
			});
			this->m_menuStrip->Location = System::Drawing::Point(0, 0);
			this->m_menuStrip->Name = L"m_menuStrip";
			this->m_menuStrip->RenderMode = System::Windows::Forms::ToolStripRenderMode::System;
			this->m_menuStrip->Size = System::Drawing::Size(384, 24);
			this->m_menuStrip->TabIndex = 10;
			this->m_menuStrip->Text = L"Menu Strip";
			// 
			// aboutToolStripMenuItem
			// 
			this->aboutToolStripMenuItem->Name = L"aboutToolStripMenuItem";
			this->aboutToolStripMenuItem->Size = System::Drawing::Size(52, 20);
			this->aboutToolStripMenuItem->Text = L"About";
			// 
			// settingsToolStripMenuItem
			// 
			this->settingsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				this->reloadObjectDatabaseToolStripMenuItem,
					this->toolStripSeparator1, this->optionsToolStripMenuItem
			});
			this->settingsToolStripMenuItem->Name = L"settingsToolStripMenuItem";
			this->settingsToolStripMenuItem->Size = System::Drawing::Size(61, 20);
			this->settingsToolStripMenuItem->Text = L"Settings";
			// 
			// reloadObjectDatabaseToolStripMenuItem
			// 
			this->reloadObjectDatabaseToolStripMenuItem->Name = L"reloadObjectDatabaseToolStripMenuItem";
			this->reloadObjectDatabaseToolStripMenuItem->Size = System::Drawing::Size(199, 22);
			this->reloadObjectDatabaseToolStripMenuItem->Text = L"Reload Object Database";
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(196, 6);
			// 
			// optionsToolStripMenuItem
			// 
			this->optionsToolStripMenuItem->Name = L"optionsToolStripMenuItem";
			this->optionsToolStripMenuItem->Size = System::Drawing::Size(199, 22);
			this->optionsToolStripMenuItem->Text = L"Options";
			// 
			// MainGui
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(384, 311);
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
			this->MinimumSize = System::Drawing::Size(400, 350);
			this->Name = L"MainGui";
			this->ShowIcon = false;
			this->Text = L"Scrap Mechanic Converter";
			this->m_menuStrip->ResumeLayout(false);
			this->m_menuStrip->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		System::Void SelectedGenerator_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
	};
}

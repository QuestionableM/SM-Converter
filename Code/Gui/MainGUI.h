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
		MainGui(void)
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
		~MainGui()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^ m_btn_folderDialog;
	protected:

	protected:
	private: System::Windows::Forms::Button^ button2;
	private: System::Windows::Forms::TextBox^ m_tb_path;
	private: System::Windows::Forms::TextBox^ m_tb_searchBox;
	private: System::Windows::Forms::ListBox^ m_lb_objectSelector;



	private: System::Windows::Forms::ComboBox^ m_cb_selectedGenerator;
	private: System::Windows::Forms::Label^ m_lbl_generatorType;
	private: System::Windows::Forms::ProgressBar^ progressBar1;
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Label^ label2;



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
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->m_tb_path = (gcnew System::Windows::Forms::TextBox());
			this->m_tb_searchBox = (gcnew System::Windows::Forms::TextBox());
			this->m_lb_objectSelector = (gcnew System::Windows::Forms::ListBox());
			this->m_cb_selectedGenerator = (gcnew System::Windows::Forms::ComboBox());
			this->m_lbl_generatorType = (gcnew System::Windows::Forms::Label());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// m_btn_folderDialog
			// 
			this->m_btn_folderDialog->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_btn_folderDialog->Location = System::Drawing::Point(328, 190);
			this->m_btn_folderDialog->Name = L"m_btn_folderDialog";
			this->m_btn_folderDialog->Size = System::Drawing::Size(44, 21);
			this->m_btn_folderDialog->TabIndex = 0;
			this->m_btn_folderDialog->Text = L"...";
			this->m_btn_folderDialog->UseVisualStyleBackColor = true;
			// 
			// button2
			// 
			this->button2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->button2->Location = System::Drawing::Point(282, 263);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(90, 36);
			this->button2->TabIndex = 1;
			this->button2->Text = L"Convert";
			this->button2->UseVisualStyleBackColor = true;
			// 
			// m_tb_path
			// 
			this->m_tb_path->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->m_tb_path->Location = System::Drawing::Point(12, 190);
			this->m_tb_path->Name = L"m_tb_path";
			this->m_tb_path->Size = System::Drawing::Size(310, 21);
			this->m_tb_path->TabIndex = 2;
			// 
			// m_tb_searchBox
			// 
			this->m_tb_searchBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_tb_searchBox->Location = System::Drawing::Point(12, 12);
			this->m_tb_searchBox->Name = L"m_tb_searchBox";
			this->m_tb_searchBox->Size = System::Drawing::Size(360, 22);
			this->m_tb_searchBox->TabIndex = 3;
			// 
			// m_lb_objectSelector
			// 
			this->m_lb_objectSelector->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lb_objectSelector->FormattingEnabled = true;
			this->m_lb_objectSelector->IntegralHeight = false;
			this->m_lb_objectSelector->ItemHeight = 16;
			this->m_lb_objectSelector->Location = System::Drawing::Point(12, 40);
			this->m_lb_objectSelector->Name = L"m_lb_objectSelector";
			this->m_lb_objectSelector->Size = System::Drawing::Size(360, 132);
			this->m_lb_objectSelector->TabIndex = 4;
			// 
			// m_cb_selectedGenerator
			// 
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
			this->m_cb_selectedGenerator->SelectedIndexChanged += gcnew System::EventHandler(this, &MainGui::m_cb_selectedGenerator_SelectedIndexChanged);
			// 
			// m_lbl_generatorType
			// 
			this->m_lbl_generatorType->AutoSize = true;
			this->m_lbl_generatorType->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lbl_generatorType->Location = System::Drawing::Point(9, 214);
			this->m_lbl_generatorType->Name = L"m_lbl_generatorType";
			this->m_lbl_generatorType->Size = System::Drawing::Size(102, 16);
			this->m_lbl_generatorType->TabIndex = 6;
			this->m_lbl_generatorType->Text = L"Generator Type";
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(12, 284);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(264, 15);
			this->progressBar1->TabIndex = 7;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(0, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(35, 13);
			this->label1->TabIndex = 8;
			this->label1->Text = L"label1";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->label2->Location = System::Drawing::Point(9, 265);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(104, 16);
			this->label2->TabIndex = 9;
			this->label2->Text = L"STATUS_TEXT";
			// 
			// MainGui
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(384, 311);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->progressBar1);
			this->Controls->Add(this->m_lbl_generatorType);
			this->Controls->Add(this->m_cb_selectedGenerator);
			this->Controls->Add(this->m_lb_objectSelector);
			this->Controls->Add(this->m_tb_searchBox);
			this->Controls->Add(this->m_tb_path);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->m_btn_folderDialog);
			this->MinimumSize = System::Drawing::Size(400, 350);
			this->Name = L"MainGui";
			this->ShowIcon = false;
			this->Text = L"Scrap Mechanic Converter";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		System::Void m_cb_selectedGenerator_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
	};
}

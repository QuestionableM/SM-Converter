#pragma once

namespace SMConverter {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for AboutGui
	/// </summary>
	public ref class AboutGui : public System::Windows::Forms::Form
	{
	public:
		AboutGui(void);

	protected:
		~AboutGui();
	private: System::Windows::Forms::TextBox^ m_tb_description;
	protected:


	private: System::Windows::Forms::Label^ m_lbl_version;
	private: System::Windows::Forms::Label^ m_lbl_createdBy;
	private: System::Windows::Forms::LinkLabel^ m_llbl_hardcoded;
	private: System::Windows::Forms::LinkLabel^ m_llbl_questionablem;
	protected:

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
			this->m_tb_description = (gcnew System::Windows::Forms::TextBox());
			this->m_lbl_version = (gcnew System::Windows::Forms::Label());
			this->m_lbl_createdBy = (gcnew System::Windows::Forms::Label());
			this->m_llbl_hardcoded = (gcnew System::Windows::Forms::LinkLabel());
			this->m_llbl_questionablem = (gcnew System::Windows::Forms::LinkLabel());
			this->SuspendLayout();
			// 
			// m_tb_description
			// 
			this->m_tb_description->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_tb_description->BackColor = System::Drawing::SystemColors::Menu;
			this->m_tb_description->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->m_tb_description->Cursor = System::Windows::Forms::Cursors::Default;
			this->m_tb_description->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_tb_description->Location = System::Drawing::Point(12, 12);
			this->m_tb_description->Multiline = true;
			this->m_tb_description->Name = L"m_tb_description";
			this->m_tb_description->ReadOnly = true;
			this->m_tb_description->Size = System::Drawing::Size(375, 41);
			this->m_tb_description->TabIndex = 0;
			this->m_tb_description->TabStop = false;
			this->m_tb_description->Text = L"SM Converter allows you to convert blueprints, tiles and world scripts from Scrap"
				L" Mechanic";
			this->m_tb_description->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &AboutGui::AboutDesc_KillFocus);
			this->m_tb_description->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &AboutGui::AboutDesc_KillFocus);
			this->m_tb_description->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &AboutGui::AboutDesc_KillFocus);
			this->m_tb_description->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &AboutGui::AboutDesc_KillFocus);
			// 
			// m_lbl_version
			// 
			this->m_lbl_version->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->m_lbl_version->AutoSize = true;
			this->m_lbl_version->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lbl_version->Location = System::Drawing::Point(9, 88);
			this->m_lbl_version->Name = L"m_lbl_version";
			this->m_lbl_version->Size = System::Drawing::Size(86, 16);
			this->m_lbl_version->TabIndex = 1;
			this->m_lbl_version->Text = L"Version: 1.0.0";
			// 
			// m_lbl_createdBy
			// 
			this->m_lbl_createdBy->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->m_lbl_createdBy->AutoSize = true;
			this->m_lbl_createdBy->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lbl_createdBy->Location = System::Drawing::Point(9, 104);
			this->m_lbl_createdBy->Name = L"m_lbl_createdBy";
			this->m_lbl_createdBy->Size = System::Drawing::Size(76, 16);
			this->m_lbl_createdBy->TabIndex = 2;
			this->m_lbl_createdBy->Text = L"Created by:";
			// 
			// m_llbl_hardcoded
			// 
			this->m_llbl_hardcoded->AutoSize = true;
			this->m_llbl_hardcoded->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_llbl_hardcoded->Location = System::Drawing::Point(9, 136);
			this->m_llbl_hardcoded->Name = L"m_llbl_hardcoded";
			this->m_llbl_hardcoded->Size = System::Drawing::Size(130, 16);
			this->m_llbl_hardcoded->TabIndex = 3;
			this->m_llbl_hardcoded->TabStop = true;
			this->m_llbl_hardcoded->Text = L"Kariaro (Hardcoded)";
			this->m_llbl_hardcoded->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &AboutGui::Creator_Hardcoded_LinkClicked);
			// 
			// m_llbl_questionablem
			// 
			this->m_llbl_questionablem->AutoSize = true;
			this->m_llbl_questionablem->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_llbl_questionablem->Location = System::Drawing::Point(9, 120);
			this->m_llbl_questionablem->Name = L"m_llbl_questionablem";
			this->m_llbl_questionablem->Size = System::Drawing::Size(120, 16);
			this->m_llbl_questionablem->TabIndex = 4;
			this->m_llbl_questionablem->TabStop = true;
			this->m_llbl_questionablem->Text = L"Questionable Mark";
			this->m_llbl_questionablem->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &AboutGui::Creator_QuestionableMark_LinkClicked);
			// 
			// AboutGui
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(399, 161);
			this->Controls->Add(this->m_llbl_questionablem);
			this->Controls->Add(this->m_llbl_hardcoded);
			this->Controls->Add(this->m_lbl_createdBy);
			this->Controls->Add(this->m_lbl_version);
			this->Controls->Add(this->m_tb_description);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"AboutGui";
			this->ShowIcon = false;
			this->Text = L"About";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		System::Void Creator_Hardcoded_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e);
		System::Void Creator_QuestionableMark_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e);
		System::Void AboutDesc_KillFocus(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
	};
}

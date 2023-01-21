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

	private:
		System::Windows::Forms::TextBox^ m_tb_description;

		System::Windows::Forms::Label^ m_lbl_version;
		System::Windows::Forms::Label^ m_lbl_createdBy;

		//Creators
		System::Windows::Forms::LinkLabel^ m_llbl_hardcoded;
		System::Windows::Forms::LinkLabel^ m_llbl_questionablem;

		//Libraries
		System::Windows::Forms::LinkLabel^ m_llbl_nlohmannJson;
		System::Windows::Forms::LinkLabel^ m_llbl_perlinNoise;
		System::Windows::Forms::LinkLabel^ m_llbl_freeImage;
		System::Windows::Forms::LinkLabel^ m_llbl_simdjson;
		System::Windows::Forms::LinkLabel^ m_llbl_valveVdf;
		System::Windows::Forms::LinkLabel^ m_llbl_assimp;
		System::Windows::Forms::LinkLabel^ m_llbl_glm;
		System::Windows::Forms::LinkLabel^ m_llbl_lz4;

		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->m_tb_description = (gcnew System::Windows::Forms::TextBox());
			this->m_lbl_version = (gcnew System::Windows::Forms::Label());
			this->m_lbl_createdBy = (gcnew System::Windows::Forms::Label());
			this->m_llbl_hardcoded = (gcnew System::Windows::Forms::LinkLabel());
			this->m_llbl_questionablem = (gcnew System::Windows::Forms::LinkLabel());
			this->m_llbl_assimp = (gcnew System::Windows::Forms::LinkLabel());
			this->m_llbl_freeImage = (gcnew System::Windows::Forms::LinkLabel());
			this->m_llbl_glm = (gcnew System::Windows::Forms::LinkLabel());
			this->m_llbl_lz4 = (gcnew System::Windows::Forms::LinkLabel());
			this->m_llbl_nlohmannJson = (gcnew System::Windows::Forms::LinkLabel());
			this->m_llbl_perlinNoise = (gcnew System::Windows::Forms::LinkLabel());
			this->m_llbl_simdjson = (gcnew System::Windows::Forms::LinkLabel());
			this->m_llbl_valveVdf = (gcnew System::Windows::Forms::LinkLabel());
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
			// m_llbl_assimp
			// 
			this->m_llbl_assimp->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->m_llbl_assimp->AutoSize = true;
			this->m_llbl_assimp->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_llbl_assimp->Location = System::Drawing::Point(335, 88);
			this->m_llbl_assimp->Name = L"m_llbl_assimp";
			this->m_llbl_assimp->Size = System::Drawing::Size(52, 16);
			this->m_llbl_assimp->TabIndex = 5;
			this->m_llbl_assimp->TabStop = true;
			this->m_llbl_assimp->Text = L"Assimp";
			this->m_llbl_assimp->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &AboutGui::LinkLabel_Assimp_LinkClicked);
			// 
			// m_llbl_freeImage
			// 
			this->m_llbl_freeImage->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->m_llbl_freeImage->AutoSize = true;
			this->m_llbl_freeImage->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_llbl_freeImage->Location = System::Drawing::Point(201, 136);
			this->m_llbl_freeImage->Name = L"m_llbl_freeImage";
			this->m_llbl_freeImage->Size = System::Drawing::Size(73, 16);
			this->m_llbl_freeImage->TabIndex = 6;
			this->m_llbl_freeImage->TabStop = true;
			this->m_llbl_freeImage->Text = L"FreeImage";
			this->m_llbl_freeImage->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &AboutGui::LinkLabel_FreeImage_LinkClicked);
			// 
			// m_llbl_glm
			// 
			this->m_llbl_glm->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->m_llbl_glm->AutoSize = true;
			this->m_llbl_glm->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_llbl_glm->Location = System::Drawing::Point(274, 104);
			this->m_llbl_glm->Name = L"m_llbl_glm";
			this->m_llbl_glm->Size = System::Drawing::Size(35, 16);
			this->m_llbl_glm->TabIndex = 7;
			this->m_llbl_glm->TabStop = true;
			this->m_llbl_glm->Text = L"GLM";
			this->m_llbl_glm->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &AboutGui::LinkLabel_GLM_LinkClicked);
			// 
			// m_llbl_lz4
			// 
			this->m_llbl_lz4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->m_llbl_lz4->AutoSize = true;
			this->m_llbl_lz4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_llbl_lz4->Location = System::Drawing::Point(300, 88);
			this->m_llbl_lz4->Name = L"m_llbl_lz4";
			this->m_llbl_lz4->Size = System::Drawing::Size(29, 16);
			this->m_llbl_lz4->TabIndex = 8;
			this->m_llbl_lz4->TabStop = true;
			this->m_llbl_lz4->Text = L"LZ4";
			this->m_llbl_lz4->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &AboutGui::LinkLabel_LZ4_LinkClicked);
			// 
			// m_llbl_nlohmannJson
			// 
			this->m_llbl_nlohmannJson->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->m_llbl_nlohmannJson->AutoSize = true;
			this->m_llbl_nlohmannJson->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_llbl_nlohmannJson->Location = System::Drawing::Point(280, 136);
			this->m_llbl_nlohmannJson->Name = L"m_llbl_nlohmannJson";
			this->m_llbl_nlohmannJson->Size = System::Drawing::Size(107, 16);
			this->m_llbl_nlohmannJson->TabIndex = 9;
			this->m_llbl_nlohmannJson->TabStop = true;
			this->m_llbl_nlohmannJson->Text = L"Nlohmann JSON";
			this->m_llbl_nlohmannJson->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &AboutGui::LinkLabel_NlohmannJson_LinkClicked);
			// 
			// m_llbl_perlinNoise
			// 
			this->m_llbl_perlinNoise->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->m_llbl_perlinNoise->AutoSize = true;
			this->m_llbl_perlinNoise->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_llbl_perlinNoise->Location = System::Drawing::Point(310, 120);
			this->m_llbl_perlinNoise->Name = L"m_llbl_perlinNoise";
			this->m_llbl_perlinNoise->Size = System::Drawing::Size(77, 16);
			this->m_llbl_perlinNoise->TabIndex = 10;
			this->m_llbl_perlinNoise->TabStop = true;
			this->m_llbl_perlinNoise->Text = L"PerlinNoise";
			this->m_llbl_perlinNoise->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &AboutGui::LinkLabel_PerlinNoise_LinkClicked);
			// 
			// m_llbl_simdjson
			// 
			this->m_llbl_simdjson->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->m_llbl_simdjson->AutoSize = true;
			this->m_llbl_simdjson->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_llbl_simdjson->Location = System::Drawing::Point(241, 120);
			this->m_llbl_simdjson->Name = L"m_llbl_simdjson";
			this->m_llbl_simdjson->Size = System::Drawing::Size(63, 16);
			this->m_llbl_simdjson->TabIndex = 11;
			this->m_llbl_simdjson->TabStop = true;
			this->m_llbl_simdjson->Text = L"Simdjson";
			this->m_llbl_simdjson->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &AboutGui::LinkLabel_Simdjson_LinkClicked);
			// 
			// m_llbl_valveVdf
			// 
			this->m_llbl_valveVdf->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->m_llbl_valveVdf->AutoSize = true;
			this->m_llbl_valveVdf->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_llbl_valveVdf->Location = System::Drawing::Point(315, 104);
			this->m_llbl_valveVdf->Name = L"m_llbl_valveVdf";
			this->m_llbl_valveVdf->Size = System::Drawing::Size(72, 16);
			this->m_llbl_valveVdf->TabIndex = 12;
			this->m_llbl_valveVdf->TabStop = true;
			this->m_llbl_valveVdf->Text = L"Valve VDF";
			this->m_llbl_valveVdf->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &AboutGui::LinkLabel_ValveVDF_LinkClicked);
			// 
			// AboutGui
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(399, 161);
			this->Controls->Add(this->m_llbl_valveVdf);
			this->Controls->Add(this->m_llbl_simdjson);
			this->Controls->Add(this->m_llbl_perlinNoise);
			this->Controls->Add(this->m_llbl_nlohmannJson);
			this->Controls->Add(this->m_llbl_lz4);
			this->Controls->Add(this->m_llbl_glm);
			this->Controls->Add(this->m_llbl_freeImage);
			this->Controls->Add(this->m_llbl_assimp);
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

		System::Void LinkLabel_NlohmannJson_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e);
		System::Void LinkLabel_FreeImage_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e);
		System::Void LinkLabel_PerlinNoise_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e);
		System::Void LinkLabel_Simdjson_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e);
		System::Void LinkLabel_ValveVDF_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e);
		System::Void LinkLabel_GLM_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e);
		System::Void LinkLabel_Assimp_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e);
		System::Void LinkLabel_LZ4_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e);
	};
}

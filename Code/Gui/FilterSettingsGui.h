#pragma once

namespace SMConverter
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class FilterSettingsGui : public System::Windows::Forms::Form
	{
	public:
		FilterSettingsGui(void);

		System::Windows::Forms::CheckBox^ m_cb_showWorkshopItems;
		System::Windows::Forms::CheckBox^ m_cb_showGameItems;
		System::Windows::Forms::CheckBox^ m_cb_showUserItems;
		System::Windows::Forms::ComboBox^ m_cb_tileSize;

		unsigned char m_tileSizeFilterCopy = 0;
		unsigned char m_userDataFilterCopy = 0;
		bool m_shouldReload = false;

	protected:
		~FilterSettingsGui();

	private:
		System::Windows::Forms::Label^ m_lbl_tileSzFilter;
		
		System::Windows::Forms::GroupBox^ m_gb_commonFilters;
		System::Windows::Forms::GroupBox^ m_gb_tileFilters;

		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->m_cb_showWorkshopItems = (gcnew System::Windows::Forms::CheckBox());
			this->m_cb_showGameItems = (gcnew System::Windows::Forms::CheckBox());
			this->m_cb_showUserItems = (gcnew System::Windows::Forms::CheckBox());
			this->m_cb_tileSize = (gcnew System::Windows::Forms::ComboBox());
			this->m_lbl_tileSzFilter = (gcnew System::Windows::Forms::Label());
			this->m_gb_commonFilters = (gcnew System::Windows::Forms::GroupBox());
			this->m_gb_tileFilters = (gcnew System::Windows::Forms::GroupBox());
			this->m_gb_commonFilters->SuspendLayout();
			this->m_gb_tileFilters->SuspendLayout();
			this->SuspendLayout();
			// 
			// m_cb_showWorkshopItems
			// 
			this->m_cb_showWorkshopItems->AutoSize = true;
			this->m_cb_showWorkshopItems->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_showWorkshopItems->Location = System::Drawing::Point(6, 19);
			this->m_cb_showWorkshopItems->Name = L"m_cb_showWorkshopItems";
			this->m_cb_showWorkshopItems->Size = System::Drawing::Size(159, 20);
			this->m_cb_showWorkshopItems->TabIndex = 0;
			this->m_cb_showWorkshopItems->Text = L"Show Workshop Items";
			this->m_cb_showWorkshopItems->UseVisualStyleBackColor = true;
			// 
			// m_cb_showGameItems
			// 
			this->m_cb_showGameItems->AutoSize = true;
			this->m_cb_showGameItems->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_showGameItems->Location = System::Drawing::Point(6, 45);
			this->m_cb_showGameItems->Name = L"m_cb_showGameItems";
			this->m_cb_showGameItems->Size = System::Drawing::Size(134, 20);
			this->m_cb_showGameItems->TabIndex = 1;
			this->m_cb_showGameItems->Text = L"Show Game Items";
			this->m_cb_showGameItems->UseVisualStyleBackColor = true;
			// 
			// m_cb_showUserItems
			// 
			this->m_cb_showUserItems->AutoSize = true;
			this->m_cb_showUserItems->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_showUserItems->Location = System::Drawing::Point(6, 71);
			this->m_cb_showUserItems->Name = L"m_cb_showUserItems";
			this->m_cb_showUserItems->Size = System::Drawing::Size(126, 20);
			this->m_cb_showUserItems->TabIndex = 2;
			this->m_cb_showUserItems->Text = L"Show User Items";
			this->m_cb_showUserItems->UseVisualStyleBackColor = true;
			// 
			// m_cb_tileSize
			// 
			this->m_cb_tileSize->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_cb_tileSize->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->m_cb_tileSize->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_cb_tileSize->FormattingEnabled = true;
			this->m_cb_tileSize->Items->AddRange(gcnew cli::array< System::Object^  >(5) {
				L"Show All Tiles", L"Show Small Tiles", L"Show Medium Tiles",
					L"Show Large Tiles", L"Show Extra Large Tiles"
			});
			this->m_cb_tileSize->Location = System::Drawing::Point(6, 35);
			this->m_cb_tileSize->Name = L"m_cb_tileSize";
			this->m_cb_tileSize->Size = System::Drawing::Size(248, 24);
			this->m_cb_tileSize->TabIndex = 3;
			// 
			// m_lbl_tileSzFilter
			// 
			this->m_lbl_tileSzFilter->AutoSize = true;
			this->m_lbl_tileSzFilter->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F));
			this->m_lbl_tileSzFilter->Location = System::Drawing::Point(3, 16);
			this->m_lbl_tileSzFilter->Name = L"m_lbl_tileSzFilter";
			this->m_lbl_tileSzFilter->Size = System::Drawing::Size(91, 16);
			this->m_lbl_tileSzFilter->TabIndex = 4;
			this->m_lbl_tileSzFilter->Text = L"Tile Size Filter";
			// 
			// m_gb_commonFilters
			// 
			this->m_gb_commonFilters->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_gb_commonFilters->Controls->Add(this->m_cb_showWorkshopItems);
			this->m_gb_commonFilters->Controls->Add(this->m_cb_showGameItems);
			this->m_gb_commonFilters->Controls->Add(this->m_cb_showUserItems);
			this->m_gb_commonFilters->Location = System::Drawing::Point(12, 12);
			this->m_gb_commonFilters->Name = L"m_gb_commonFilters";
			this->m_gb_commonFilters->Size = System::Drawing::Size(260, 97);
			this->m_gb_commonFilters->TabIndex = 5;
			this->m_gb_commonFilters->TabStop = false;
			this->m_gb_commonFilters->Text = L"Common Filters";
			// 
			// m_gb_tileFilters
			// 
			this->m_gb_tileFilters->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_gb_tileFilters->Controls->Add(this->m_lbl_tileSzFilter);
			this->m_gb_tileFilters->Controls->Add(this->m_cb_tileSize);
			this->m_gb_tileFilters->Location = System::Drawing::Point(12, 115);
			this->m_gb_tileFilters->Name = L"m_gb_tileFilters";
			this->m_gb_tileFilters->Size = System::Drawing::Size(260, 65);
			this->m_gb_tileFilters->TabIndex = 6;
			this->m_gb_tileFilters->TabStop = false;
			this->m_gb_tileFilters->Text = L"Tile Filters";
			// 
			// FilterSettingsGui
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 192);
			this->Controls->Add(this->m_gb_tileFilters);
			this->Controls->Add(this->m_gb_commonFilters);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"FilterSettingsGui";
			this->ShowIcon = false;
			this->Text = L"Filter";
			this->m_gb_commonFilters->ResumeLayout(false);
			this->m_gb_commonFilters->PerformLayout();
			this->m_gb_tileFilters->ResumeLayout(false);
			this->m_gb_tileFilters->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
		System::Void Filter_ShowWorkshopItems_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void Filter_ShowGameItems_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void Filter_ShowUserItems_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void Filter_TileSizeFilter_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
		System::Void UpdateChangedStatus();
	};
}

#include "ItemInfoGui.h"

#include "UStd\UnmanagedUnorderedMap.hpp"
#include "UStd\UnmanagedVector.hpp"

#include "ObjectDatabase\UserDataReaders\ItemModCounter.hpp"
#include "ObjectDatabase\Mods\CustomGameSwitch.hpp"
#include "ObjectDatabase\DatabaseConfig.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Converter\WorldConverter\World.hpp"
#include "Converter\Entity\Blueprint.hpp"
#include "Utils\Console.hpp"
#include "Utils\File.hpp"

namespace SMConverter
{
	ItemInfoGui::ItemInfoGui(BlueprintInstance* pBlueprint)
	{
		ItemModStats::Reset();

		this->InitializeComponent();
		this->Text = "Blueprint Info";

		m_lbl_line1->Text += gcnew System::String(pBlueprint->name.c_str());
		m_lbl_line2->Text += gcnew System::String(pBlueprint->uuid.toString().c_str());
		
		const std::string v_filter_name(FilterSettingsData::GetFilterName(pBlueprint->filter));
		m_lbl_line3->Text = gcnew System::String(("Content Type: " + v_filter_name).c_str());

		const std::wstring& v_bp_preview = pBlueprint->preview_image;
		if (!v_bp_preview.empty() && File::Exists(v_bp_preview))
			m_bp_blueprintPreview->ImageLocation = gcnew System::String(v_bp_preview.c_str());

		SMBlueprint::CountFromFile(pBlueprint->path);

		m_lbl_line4->Text = gcnew System::String(("Part Count: " + std::to_string(ItemModStats::GetTotalPartCount())).c_str());
		m_lbl_line5->Text = gcnew System::String(("Mod Count: " + std::to_string(ItemModStats::ModStorage.size())).c_str());
		
		this->UpdateModList();
	}

	inline SMWorld* ItemInfoGui_ReadWorldData(const std::wstring& worldPath, ConvertError& v_error)
	{
		TileFolderReader::InitializeTileKeys();

		CustomGame* v_pCustomGame = SMMod::GetCustomGameFromPath(worldPath);
		if (v_pCustomGame)
		{
			SMModCustomGameSwitch<false, true> v_cg_switch;
			v_cg_switch.MergeContent(v_pCustomGame);

			SMWorld* v_out_world = SMWorld::LoadFromFile<true>(worldPath, v_error);
			return v_out_world;
		}
		else
		{
			return SMWorld::LoadFromFile<true>(worldPath, v_error);
		}
	}

	ItemInfoGui::ItemInfoGui(WorldInstance* pWorld)
	{
		ItemModStats::Reset();

		this->InitializeComponent();
		this->Text = "World Info";

		m_lbl_line1->Text += gcnew System::String(pWorld->name.c_str());
		m_lbl_line2->Text += gcnew System::String(pWorld->uuid.toString().c_str());

		ConvertError v_error;
		SMWorld* v_cur_world = ItemInfoGui_ReadWorldData(pWorld->path, v_error);

		const std::string v_world_sz_str = std::to_string(v_cur_world->GetWidth());
		m_lbl_line3->Text = gcnew System::String(("World Size: " + v_world_sz_str + "x" + v_world_sz_str).c_str());

		const std::string v_filter_name(FilterSettingsData::GetFilterName(pWorld->filter));
		m_lbl_line4->Text = gcnew System::String(("Content Type: " + v_filter_name).c_str());

		if (v_cur_world) delete v_cur_world;

		if (v_error)
		{
			System::Windows::Forms::MessageBox::Show(
				gcnew System::String(("Error: " + v_error.getErrorMsg()).c_str()),
				"World Read Error",
				System::Windows::Forms::MessageBoxButtons::OK,
				System::Windows::Forms::MessageBoxIcon::Error
			);

			m_isSuccess = false;
			return;
		}

		const std::wstring& v_world_preview = pWorld->preview_image;
		if (!v_world_preview.empty() && File::Exists(v_world_preview))
			m_bp_blueprintPreview->ImageLocation = gcnew System::String(v_world_preview.c_str());

		m_lbl_line5->Text = gcnew System::String(("Part Count: " + std::to_string(ItemModStats::GetTotalPartCount())).c_str());
		m_lbl_line6->Text = gcnew System::String(("Mod Count: " + std::to_string(ItemModStats::ModStorage.size())).c_str());
		m_lbl_line6->Visible = true;

		this->UpdateModList();
	}

	inline void ItemInfoGui_ReadTileData(const std::wstring& path, ConvertError& v_error)
	{
		CustomGame* v_pCustomGame = SMMod::GetCustomGameFromPath(path);
		if (v_pCustomGame)
		{
			SMModCustomGameSwitch<false, true> v_cg_switch;
			v_cg_switch.MergeContent(v_pCustomGame);

			TileFolderReader::GetTileData(path, v_error);
		}
		else
		{
			TileFolderReader::GetTileData(path, v_error);
		}
	}

	ItemInfoGui::ItemInfoGui(TileInstance* pTile)
	{
		ItemModStats::Reset();

		this->InitializeComponent();
		this->Text = "Tile Info";

		m_lbl_line1->Text += gcnew System::String(pTile->name.c_str());
		m_lbl_line2->Text += gcnew System::String(pTile->uuid.toString().c_str());
		
		const std::string v_size_name(FilterSettingsData::GetTileSizeName(pTile->size_filter));
		m_lbl_line3->Text = gcnew System::String(("Tile Size: " + v_size_name).c_str());

		const std::string v_filter_name(FilterSettingsData::GetFilterName(pTile->filter));
		m_lbl_line4->Text = gcnew System::String(("Content Type: " + v_filter_name).c_str());

		ConvertError v_error;
		ItemInfoGui_ReadTileData(pTile->path, v_error);

		if (v_error)
		{
			System::Windows::Forms::MessageBox::Show(
				gcnew System::String(("Error: " + v_error.getErrorMsg()).c_str()),
				"Tile Read Error",
				System::Windows::Forms::MessageBoxButtons::OK,
				System::Windows::Forms::MessageBoxIcon::Error
			);

			m_isSuccess = false;
			return;
		}

		const std::wstring& v_tile_preview = pTile->preview_image;
		if (!v_tile_preview.empty() && File::Exists(v_tile_preview))
			m_bp_blueprintPreview->ImageLocation = gcnew System::String(v_tile_preview.c_str());

		m_lbl_line5->Text = gcnew System::String(("Part Count: " + std::to_string(ItemModStats::GetTotalPartCount())).c_str());
		m_lbl_line6->Text = gcnew System::String(("Mod Count: " + std::to_string(ItemModStats::ModStorage.size())).c_str());
		m_lbl_line6->Visible = true;

		this->UpdateModList();
	}

	ItemInfoGui::~ItemInfoGui()
	{
		if (components) delete components;
	}

	void ItemInfoGui::UpdateModList()
	{
		m_lbl_noItems->Visible = ItemModStats::ModVector.empty();
		m_lb_modSelector->Enabled = !ItemModStats::ModVector.empty();

		m_lb_modSelector->BeginUpdate();
		m_lb_modSelector->Items->Clear();

		for (const ItemModInstance* v_mod_data : ItemModStats::ModVector)
		{
			std::wstring v_mod_name = (v_mod_data->m_pMod != nullptr) ? v_mod_data->m_pMod->GetName() : L"UNKNOWN_MOD";

			v_mod_name.append(L" (");
			v_mod_name.append(std::to_wstring(v_mod_data->m_partCount));
			v_mod_name.append(L")");

			m_lb_modSelector->Items->Add(gcnew System::String(v_mod_name.c_str()));
		}

		m_lb_modSelector->EndUpdate();

		this->UpdateContextMenuStrip();
	}

	SMMod* ItemInfoGui::GetCurrentMod()
	{
		if (m_lb_modSelector->SelectedIndex == -1) return nullptr;

		return ItemModStats::ModVector[m_lb_modSelector->SelectedIndex]->m_pMod;
	}

	void ItemInfoGui::ModSelector_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		if (e->Button != System::Windows::Forms::MouseButtons::Right) return;

		const int v_idx = m_lb_modSelector->IndexFromPoint(e->Location);
		if (v_idx == -1) return;

		m_lb_modSelector->SetSelected(v_idx, true);
	}

	void ItemInfoGui::ModList_OpenInSteamWorkshop_Click(System::Object^ sender, System::EventArgs^ e)
	{
		SMMod* v_mod = this->GetCurrentMod();
		if (!v_mod) return;

		const unsigned long long& v_workshop_id = v_mod->GetWorkshopId();
		if (v_workshop_id == 0) return;

		std::wstring v_workshop_url;
		if (DatabaseConfig::OpenLinksInSteam)
			v_workshop_url.append(L"steam://openurl/");

		v_workshop_url.append(L"https://steamcommunity.com/sharedfiles/filedetails/?id=");
		v_workshop_url.append(std::to_wstring(v_workshop_id));

		System::Diagnostics::Process::Start(gcnew System::String(v_workshop_url.c_str()));
	}

	void ItemInfoGui::ModList_OpenInExplorer_Click(System::Object^ sender, System::EventArgs^ e)
	{
		SMMod* v_mod = this->GetCurrentMod();
		if (!v_mod) return;

		const std::wstring v_mod_dir = ::String::ReplaceAll(v_mod->GetDirectory(), L'/', L'\\');
		if (!File::Exists(v_mod_dir))
		{
			System::Windows::Forms::MessageBox::Show(
				"Mod Folder doesn't exist!",
				"Invalid Mod",
				System::Windows::Forms::MessageBoxButtons::OK,
				System::Windows::Forms::MessageBoxIcon::Warning
			);

			return;
		}

		System::Diagnostics::Process::Start("explorer.exe", gcnew System::String(v_mod_dir.c_str()));
	}

	void ItemInfoGui::ModList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
	{
		this->UpdateContextMenuStrip();
	}

	void ItemInfoGui::UpdateContextMenuStrip()
	{
		bool v_has_workshop_id = false;
		bool v_has_directory = false;

		SMMod* v_mod = this->GetCurrentMod();
		if (v_mod)
		{
			v_has_workshop_id = (v_mod->GetWorkshopId() != 0);
			v_has_directory = File::Exists(v_mod->GetDirectory());
		}

		m_btn_openInSteamWorkshop->Enabled = v_has_workshop_id;
		m_btn_openInExplorer->Enabled = v_has_directory;
	}
}
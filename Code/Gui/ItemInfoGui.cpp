#include "ItemInfoGui.h"

#include "UStd\UnmanagedUnorderedMap.hpp"
#include "UStd\UnmanagedVector.hpp"

#include "ObjectDatabase\DatabaseConfig.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\Console.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

struct ItemInfoData
{
	inline static std::unordered_map<SMUuid, BlueprintModStats> ModStorage = {};
	inline static std::vector<BlueprintModStats> ModVector = {};
	inline static std::size_t ObjectCount = 0;
};

#pragma managed

namespace SMConverter
{
	ItemInfoGui::ItemInfoGui(BlueprintInstance* v_blueprint)
	{
		this->InitializeComponent();
		this->Text = "Blueprint Info";

		ItemInfoData::ModStorage.clear();
		ItemInfoData::ModVector.clear();
		ItemInfoData::ObjectCount = 0;

		std::wstring v_info_str;
		v_info_str.append(L"Name: " + v_blueprint->name + L"\r\n");
		v_info_str.append(L"Uuid: " + v_blueprint->uuid.ToWstring() + L"\r\n");
		
		const std::wstring v_filter_name = FilterSettingsData::GetFilterName(v_blueprint->v_filter);
		v_info_str.append(L"Content Type: " + v_filter_name + L"\r\n");

		const std::wstring& v_bp_preview = v_blueprint->preview_image;
		if (!v_bp_preview.empty() && File::Exists(v_bp_preview))
			m_bp_blueprintPreview->ImageLocation = gcnew System::String(v_bp_preview.c_str());

		BlueprintFolderReader::GetBlueprintData(v_blueprint, ItemInfoData::ModStorage, ItemInfoData::ObjectCount);

		ItemInfoData::ModVector.reserve(ItemInfoData::ModStorage.size());
		for (const auto& v_mod_item : ItemInfoData::ModStorage)
			ItemInfoData::ModVector.push_back(v_mod_item.second);

		v_info_str.append(L"Part Count: " + std::to_wstring(ItemInfoData::ObjectCount) + L"\r\n");
		v_info_str.append(L"Mod Count: " + std::to_wstring(ItemInfoData::ModStorage.size()) + L"\r\n");

		m_tb_itemInfo->Text = gcnew System::String(v_info_str.c_str());
		this->UpdateModList();
	}

	ItemInfoGui::ItemInfoGui(TileInstance* v_tile)
	{
		this->InitializeComponent();
		this->Text = "Tile Info";

		ItemInfoData::ModStorage.clear();
		ItemInfoData::ModVector.clear();
		ItemInfoData::ObjectCount = 0;

		std::wstring v_info_str;
		v_info_str.append(L"Name: " + v_tile->name + L"\r\n");
		v_info_str.append(L"Uuid: " + v_tile->uuid.ToWstring() + L"\r\n");
		
		const std::wstring v_size_name = FilterSettingsData::GetTileSizeName(v_tile->v_size_filter);
		v_info_str.append(L"Tile Size: " + v_size_name + L"\r\n");

		const std::wstring v_filter_name = FilterSettingsData::GetFilterName(v_tile->v_filter);
		v_info_str.append(L"Content Type: " + v_filter_name + L"\r\n");

		const std::wstring& v_tile_preview = v_tile->preview_image;
		if (!v_tile_preview.empty() && File::Exists(v_tile_preview))
			m_bp_blueprintPreview->ImageLocation = gcnew System::String(v_tile_preview.c_str());

		v_info_str.append(L"Part Count: NOT_IMPLEMENTED\r\n");
		v_info_str.append(L"Mod Count: NOT_IMPLEMENTED\r\n");
		
		m_tb_itemInfo->Text = gcnew System::String(v_info_str.c_str());
		this->UpdateModList();
	}

	ItemInfoGui::~ItemInfoGui()
	{
		if (components) delete components;
	}

	void ItemInfoGui::UpdateModList()
	{
		m_lb_modSelector->SelectedIndex = -1;

		m_lb_modSelector->BeginUpdate();
		m_lb_modSelector->Items->Clear();

		for (const BlueprintModStats& v_mod_data : ItemInfoData::ModVector)
		{
			std::wstring v_mod_name = (v_mod_data.mod != nullptr) ? v_mod_data.mod->GetName() : L"UNKNOWN_MOD";

			v_mod_name.append(L" (");
			v_mod_name.append(std::to_wstring(v_mod_data.part_count));
			v_mod_name.append(L")");

			m_lb_modSelector->Items->Add(gcnew System::String(v_mod_name.c_str()));
		}

		m_lb_modSelector->EndUpdate();
	}

	SMMod* ItemInfoGui::GetCurrentMod()
	{
		if (m_lb_modSelector->SelectedIndex == -1) return nullptr;

		return ItemInfoData::ModVector[m_lb_modSelector->SelectedIndex].mod;
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
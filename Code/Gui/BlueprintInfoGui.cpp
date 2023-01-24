#include "BlueprintInfoGui.h"

#include "UStd\UnmanagedUnorderedMap.hpp"
#include "UStd\UnmanagedVector.hpp"

#include "ObjectDatabase\DatabaseConfig.hpp"
#include "ObjectDatabase\Mods\Mod.hpp"

#include "Utils\Console.hpp"
#include "Utils\File.hpp"

#pragma unmanaged

struct BlueprintInfoData
{
	inline static std::unordered_map<SMUuid, BlueprintModStats> ModStorage = {};
	inline static std::vector<BlueprintModStats> ModVector = {};
	inline static std::size_t ObjectCount = 0;
};

#pragma managed

namespace SMConverter
{
	BlueprintInfoGui::BlueprintInfoGui(BlueprintInstance* v_blueprint)
	{
		this->InitializeComponent();

		m_blueprint = v_blueprint;

		m_lbl_blueprintName->Text += gcnew System::String(m_blueprint->name.c_str());
		m_lbl_blueprintUuid->Text += gcnew System::String(m_blueprint->uuid.ToString().c_str());

		const char* v_filter_name = FilterSettingsData::GetFilterName(m_blueprint->v_filter);
		m_lbl_contentType->Text += gcnew System::String(v_filter_name);

		if (!m_blueprint->preview_image.empty())
			m_bp_blueprintPreview->ImageLocation = gcnew System::String(m_blueprint->preview_image.c_str());

		BlueprintInfoData::ModStorage.clear();
		BlueprintInfoData::ModVector.clear();
		BlueprintInfoData::ObjectCount = 0;

		BlueprintFolderReader::GetBlueprintData(v_blueprint, BlueprintInfoData::ModStorage, BlueprintInfoData::ObjectCount);

		BlueprintInfoData::ModVector.reserve(BlueprintInfoData::ModStorage.size());
		for (const auto& v_mod_vector : BlueprintInfoData::ModStorage)
			BlueprintInfoData::ModVector.push_back(v_mod_vector.second);

		m_lbl_modCount->Text = gcnew System::String(("Mods: " + std::to_string(BlueprintInfoData::ModStorage.size())).c_str());
		m_lbl_partCount->Text = gcnew System::String(("Part Count: " + std::to_string(BlueprintInfoData::ObjectCount)).c_str());

		this->UpdateModList();
	}

	BlueprintInfoGui::~BlueprintInfoGui()
	{
		if (components) delete components;
	}

	void BlueprintInfoGui::UpdateModList()
	{
		m_lb_modSelector->SelectedIndex = -1;

		m_lb_modSelector->BeginUpdate();
		m_lb_modSelector->Items->Clear();

		for (const BlueprintModStats& v_mod_data : BlueprintInfoData::ModVector)
		{
			std::wstring v_mod_name = (v_mod_data.mod != nullptr) ? v_mod_data.mod->GetName() : L"UNKNOWN_MOD";

			v_mod_name.append(L" (");
			v_mod_name.append(std::to_wstring(v_mod_data.part_count));
			v_mod_name.append(L")");

			m_lb_modSelector->Items->Add(gcnew System::String(v_mod_name.c_str()));
		}

		m_lb_modSelector->EndUpdate();
	}

	SMMod* BlueprintInfoGui::GetCurrentMod()
	{
		if (m_lb_modSelector->SelectedIndex == -1) return nullptr;

		return BlueprintInfoData::ModVector[m_lb_modSelector->SelectedIndex].mod;
	}

	void BlueprintInfoGui::ModSelector_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		if (e->Button != System::Windows::Forms::MouseButtons::Right) return;

		const int v_idx = m_lb_modSelector->IndexFromPoint(e->Location);
		if (v_idx == -1) return;

		m_lb_modSelector->SetSelected(v_idx, true);
	}

	void BlueprintInfoGui::ModList_OpenInSteamWorkshop_Click(System::Object^ sender, System::EventArgs^ e)
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

	void BlueprintInfoGui::ModList_OpenInExplorer_Click(System::Object^ sender, System::EventArgs^ e)
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

	void BlueprintInfoGui::ModList_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
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
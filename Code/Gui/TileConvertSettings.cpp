#include "TileConvertSettings.h"

#include "Converter\ConvertSettings.hpp"

#include "Utils\String.hpp"
#include "Utils\File.hpp"

#include <msclr\marshal_cppstd.h>

namespace SMConverter
{
	TileConvertSettings::TileConvertSettings(const wchar_t* v_filename)
	{
		this->InitializeComponent();

		const std::wstring v_filename_limited = ::String::LimitWstring(v_filename, static_cast<std::size_t>(m_tb_filename->MaxLength));
		m_tb_filename->Text = gcnew System::String(v_filename_limited.c_str());

		m_cb_exportHarvestables->Checked = TileConverterSettings::ExportHarvestables;
		m_cb_exportBlueprints->Checked = TileConverterSettings::ExportBlueprints;
		m_cb_exportClutter->Checked = TileConverterSettings::ExportClutter;
		m_cb_exportPrefabs->Checked = TileConverterSettings::ExportPrefabs;
		m_cb_exportDecals->Checked = TileConverterSettings::ExportDecals;
		m_cb_exportAssets->Checked = TileConverterSettings::ExportAssets;

		const bool v_export_materials = SharedConverterSettings::ExportMaterials && SharedConverterSettings::ExportUvs;

		m_cb_export8kGndTextures->Checked = TileConverterSettings::ExportGroundTextures
			&& TileConverterSettings::Export8kGroundTextures && v_export_materials;

		m_cb_exportGndTextures->Checked = TileConverterSettings::ExportGroundTextures && v_export_materials;
		m_cb_exportMaterials->Checked = v_export_materials;

		m_cb_exportNormals->Checked = SharedConverterSettings::ExportNormals;
		m_cb_exportUvs->Checked = SharedConverterSettings::ExportUvs;

		SendMessage(
			static_cast<HWND>(m_tb_filename->Handle.ToPointer()),
			EM_SETCUEBANNER,
			0,
			reinterpret_cast<LPARAM>(L"Enter a tile name")
		);
	}

	TileConvertSettings::~TileConvertSettings()
	{
		if (components) delete components;
	}

	void TileConvertSettings::FileName_TextChanged(System::Object^ sender, System::EventArgs^ e)
	{
		m_btn_convert->Enabled = (m_tb_filename->TextLength > 0);
	}

	void TileConvertSettings::FileName_KeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e)
	{
		if (!::String::IsPathCharacterAllowed(e->KeyChar) && e->KeyChar != 0x8 && e->KeyChar != 0x1)
			e->Handled = true;
	}

	void TileConvertSettings::ConvertButton_Click(System::Object^ sender, System::EventArgs^ e)
	{
		const std::wstring v_tile_name = msclr::interop::marshal_as<std::wstring>(m_tb_filename->Text);

		if (!File::IsPathLegal(L"./" + v_tile_name))
		{
			System::Windows::Forms::MessageBox::Show(
				"The specified file name is invalid!",
				"Invalid Filename",
				System::Windows::Forms::MessageBoxButtons::OK,
				System::Windows::Forms::MessageBoxIcon::Warning
			);

			return;
		}

		m_ready_to_convert = true;
		this->Close();
	}

	void TileConvertSettings::ExportGroundTextures_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		m_cb_export8kGndTextures->Enabled = m_cb_exportGndTextures->Checked;

		if (!m_cb_exportGndTextures->Checked)
			m_cb_export8kGndTextures->Checked = false;
	}

	void TileConvertSettings::ExportUvCoords_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		m_cb_exportMaterials->Enabled = m_cb_exportUvs->Checked;

		if (!m_cb_exportUvs->Checked)
			m_cb_exportMaterials->Checked = false;
	}
}
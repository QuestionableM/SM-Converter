#include "TileConvertSettings.h"

#include "Converter\ConvertSettings.hpp"

namespace SMConverter
{
	TileConvertSettings::TileConvertSettings(const wchar_t* v_filename)
	{
		this->InitializeComponent();

		m_tb_filename->Text = gcnew System::String(v_filename);

		m_cb_exportHarvestables->Checked = TileConverterSettings::ExportHarvestables;
		m_cb_exportBlueprints->Checked = TileConverterSettings::ExportBlueprints;
		m_cb_exportClutter->Checked = TileConverterSettings::ExportClutter;
		m_cb_exportPrefabs->Checked = TileConverterSettings::ExportPrefabs;
		m_cb_exportDecals->Checked = TileConverterSettings::ExportDecals;
		m_cb_exportAssets->Checked = TileConverterSettings::ExportAssets;

		m_cb_export8kGndTextures->Checked = TileConverterSettings::ExportGroundTextures
			&& TileConverterSettings::Export8kGroundTextures && SharedConverterSettings::ExportMaterials;

		m_cb_exportGndTextures->Checked = TileConverterSettings::ExportGroundTextures && SharedConverterSettings::ExportMaterials;
		m_cb_exportMaterials->Checked = SharedConverterSettings::ExportMaterials;

		m_cb_exportNormals->Checked = SharedConverterSettings::ExportNormals;
		m_cb_exportUvs->Checked = SharedConverterSettings::ExportUvs;
	}

	TileConvertSettings::~TileConvertSettings()
	{
		if (components) delete components;
	}

	void TileConvertSettings::FileName_TextChanged(System::Object^ sender, System::EventArgs^ e)
	{
		m_btn_convert->Enabled = (m_tb_filename->TextLength > 0);
	}

	void TileConvertSettings::ConvertButton_Click(System::Object^ sender, System::EventArgs^ e)
	{
		m_ready_to_convert = true;
		this->Close();
	}
}
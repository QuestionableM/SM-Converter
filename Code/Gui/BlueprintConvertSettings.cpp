#include "BlueprintConvertSettings.h"

#include "Converter\ConvertSettings.hpp"

#include "Utils\String.hpp"
#include "Utils\File.hpp"

#include <msclr\marshal_cppstd.h>

namespace SMConverter
{
	BlueprintConvertSettings::BlueprintConvertSettings(const wchar_t* v_filename)
	{
		this->InitializeComponent();

		const std::wstring v_limited_filename = ::String::LimitWstring(v_filename, static_cast<std::size_t>(m_tb_filename->MaxLength));
		m_tb_filename->Text = gcnew System::String(v_limited_filename.c_str());

		m_cb_exportMaterials->Checked = SharedConverterSettings::ExportMaterials && SharedConverterSettings::ExportUvs;
		m_cb_exportUvs->Checked = SharedConverterSettings::ExportUvs;
		m_cb_exportNormals->Checked = SharedConverterSettings::ExportNormals;

		m_cb_separationType->SelectedIndex = BlueprintConverterSettings::SeparationType;

		SendMessage(
			static_cast<HWND>(m_tb_filename->Handle.ToPointer()),
			EM_SETCUEBANNER,
			0,
			reinterpret_cast<LPARAM>(L"Enter a blueprint name")
		);
	}

	BlueprintConvertSettings::~BlueprintConvertSettings()
	{
		if (components) delete components;
	}

	void BlueprintConvertSettings::FileName_TextChanged(System::Object^ sender, System::EventArgs^ e)
	{
		m_btn_convert->Enabled = (m_tb_filename->TextLength > 0);
	}

	void BlueprintConvertSettings::FileName_KeyPress(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e)
	{
		if (!::String::IsPathCharacterAllowed(e->KeyChar) && e->KeyChar != 0x8 && e->KeyChar != 0x1)
			e->Handled = true;
	}

	void BlueprintConvertSettings::ExportUvCoords_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		m_cb_exportMaterials->Enabled = m_cb_exportUvs->Checked;

		if (!m_cb_exportUvs->Checked)
			m_cb_exportMaterials->Checked = false;
	}

	void BlueprintConvertSettings::ConvertButton_Click(System::Object^ sender, System::EventArgs^ e)
	{
		const std::wstring v_bp_name = msclr::interop::marshal_as<std::wstring>(m_tb_filename->Text);

		if (!File::IsPathLegal(L"./" + v_bp_name))
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
}
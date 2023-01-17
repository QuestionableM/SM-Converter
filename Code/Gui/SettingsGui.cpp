#include "SettingsGui.h"

#include "Utils\File.hpp"

namespace SMConverter
{
	SettingsGui::SettingsGui()
	{
		this->InitializeComponent();
	}

	SettingsGui::~SettingsGui()
	{
		if (components) delete components;
	}

	void SettingsGui::Settings_PathSelector_Click(System::Object^ sender, System::EventArgs^ e)
	{
		const std::wstring v_file_path = File::OpenFileDialog(
			L"Select a folder",
			FOS_FILEMUSTEXIST | FOS_PICKFOLDERS,
			L"All Files (*.*)\0*.*\0",
			static_cast<HWND>(this->Handle.ToPointer())
		);

		if (v_file_path.empty())
			return;

		m_tb_gamePath->Text = gcnew System::String(v_file_path.c_str());
	}
}
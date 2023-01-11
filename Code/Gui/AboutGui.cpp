#include "AboutGui.h"

#include "BuildInfo.hpp"

#define STR_INDIR(x) #x
#define TO_STR(x) STR_INDIR(x)

#include <Windows.h>

namespace SMConverter
{
	AboutGui::AboutGui()
	{
		this->InitializeComponent();

		m_lbl_version->Text += "." TO_STR(SM_CONVERTER_BUILD_VERSION);

	#if defined(_WIN64)
		m_lbl_version->Text += " (x64)";
	#else
		m_lbl_version->Text += " (x86)";
	#endif

	#if defined(DEBUG) || defined(_DEBUG)
		m_lbl_version->Text += " DEBUG";
	#endif
	}

	AboutGui::~AboutGui()
	{
		if (components) delete components;
	}

	void AboutGui::Creator_Hardcoded_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e)
	{
		System::Diagnostics::Process::Start("https://github.com/Kariaro");
	}

	void AboutGui::Creator_QuestionableMark_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e)
	{
		System::Diagnostics::Process::Start("https://github.com/QuestionableM");
	}

	void AboutGui::AboutDesc_KillFocus(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
	{
		SendMessage(
			static_cast<HWND>(m_tb_description->Handle.ToPointer()),
			WM_KILLFOCUS,
			0,
			0
		);
	}
}
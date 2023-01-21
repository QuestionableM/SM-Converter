#include "AboutGui.h"

#include "Utils\WinInclude.hpp"
#include "BuildInfo.hpp"

#define STR_INDIR(x) #x
#define TO_STR(x) STR_INDIR(x)

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



	void AboutGui::LinkLabel_NlohmannJson_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e)
	{
		System::Diagnostics::Process::Start("https://github.com/nlohmann/json");
	}

	void AboutGui::LinkLabel_FreeImage_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e)
	{
		System::Diagnostics::Process::Start("https://freeimage.sourceforge.io/");
	}

	void AboutGui::LinkLabel_PerlinNoise_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e)
	{
		System::Diagnostics::Process::Start("https://github.com/Reputeless/PerlinNoise");
	}

	void AboutGui::LinkLabel_Simdjson_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e)
	{
		System::Diagnostics::Process::Start("https://github.com/simdjson/simdjson");
	}

	void AboutGui::LinkLabel_ValveVDF_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e)
	{
		System::Diagnostics::Process::Start("https://github.com/TinyTinni/ValveFileVDF");
	}

	void AboutGui::LinkLabel_GLM_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e)
	{
		System::Diagnostics::Process::Start("https://github.com/g-truc/glm");
	}

	void AboutGui::LinkLabel_Assimp_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e)
	{
		System::Diagnostics::Process::Start("https://github.com/assimp/assimp");
	}

	void AboutGui::LinkLabel_LZ4_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e)
	{
		System::Diagnostics::Process::Start("https://github.com/lz4/lz4");
	}
}
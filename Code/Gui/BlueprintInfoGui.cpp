#include "BlueprintInfoGui.h"

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
	}

	BlueprintInfoGui::~BlueprintInfoGui()
	{
		if (components) delete components;
	}
}
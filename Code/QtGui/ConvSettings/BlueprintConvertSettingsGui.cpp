#include "BlueprintConvertSettingsGui.hpp"

#include "Converter/ConvertSettings.hpp"
#include "QtGui/QtUiConstants.hpp"

//////////BLUEPRINT CONVERTER THREAD DATA//////////

void BlueprintConverterThreadData::applySettings() const
{
	this->applySettingsBase();
	BlueprintConverterSettings::SeparationType = this->separation_type;
	BlueprintConverterSettings::ApplyControllerPresets = this->apply_controller_presets;
}

//////////BLUEPRINT CONVERTER SETTINGS GUI/////////

BlueprintConverterSettingsGui::BlueprintConverterSettingsGui(
	QWidget* parent,
	const std::wstring& filename,
	const std::wstring& path
)
	: ConverterSettingsGuiBase(parent, filename)
	, m_cbSeparationType(this)
	, m_blueprintOptions(this)
	, m_modelSettings(this)
	, m_cbCustomGameContent(this, path)
{
	this->setWindowFlags(this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	this->setWindowTitle("Blueprint Convert Settings");
	this->setFixedWidth(350);

	m_objectName.setTitle("Blueprint Name");

	m_cbSeparationType.setTitle("Separation Type");
	m_cbSeparationType.m_comboBox->addItems(
		{
			"None",
			"Separate All",
			"Separate by Shapes",
			"Separate by Joints",
			"Group Objects by UUID",
			"Group Objects by Color",
			"Group Objects by UUID and Color"
		}
	);
	m_cbSeparationType.m_comboBox->setCurrentIndex(
		BlueprintConverterSettings::SeparationType);

	m_blueprintOptions.setTitle("Blueprint Options");
	m_bApplyControllerPresets = new QCheckBox("Apply Controller Presets", &m_blueprintOptions);
	m_bApplyControllerPresets->setChecked(BlueprintConverterSettings::ApplyControllerPresets);
	m_blueprintOptions.m_layout->addWidget(m_bApplyControllerPresets);

	m_objectLayout->addWidget(&m_objectName, 0, Qt::AlignTop);
	m_objectLayout->addWidget(&m_cbSeparationType, 0, Qt::AlignTop);
	m_objectLayout->addWidget(&m_blueprintOptions, 0, Qt::AlignTop);
	m_objectLayout->addWidget(&m_modelSettings, 0, Qt::AlignTop);
	m_objectLayout->addWidget(&m_cbCustomGameContent, 0, Qt::AlignTop);
	m_objectLayout->addWidget(m_convertButton, 0, Qt::AlignBottom);

	this->adjustSize();
	this->setMinimumSize(this->size());
}

void BlueprintConverterSettingsGui::getThreadData(BlueprintConverterThreadData* data)
{
	data->name = m_objectName.m_lineEdit->text().toStdWString();
	
	data->custom_game_idx.value = m_cbCustomGameContent.getCustomGameIndex();
	data->separation_type = m_cbSeparationType.m_comboBox->currentIndex();
	data->apply_controller_presets = m_bApplyControllerPresets->isChecked();

	m_modelSettings.fillSettings(&data->model_settings);
}
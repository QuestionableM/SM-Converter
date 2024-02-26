#include "ConverterSettingsGuiBase.hpp"

#include "Converter/ConvertSettings.hpp"
#include "QtUiConstants.hpp"

#include "ObjectDatabase/Mods/Mod.hpp"

#include "Utils/Console.hpp"
#include "Utils/String.hpp"

void ModelExportSettings::apply() const
{
	SharedConverterSettings::ExportMaterials = this->export_materials && this->export_uvs;
	SharedConverterSettings::ExportNormals = this->export_normals;
	SharedConverterSettings::ExportUvs = this->export_uvs;
}

CustomGame* CustomGameIndex::getPtr() const
{
	if (this->value == -1)
		return nullptr;

	const auto& v_custom_games = SMMod::GetCustomGames();
	const std::size_t v_custom_game_count = v_custom_games.size();
	const std::size_t v_custom_game_idx = std::size_t(this->value);

	if (v_custom_game_idx >= v_custom_game_count)
		return nullptr;

	return v_custom_games[v_custom_game_idx];
}

void ConverterThreadDataBase::applySettingsBase() const
{
	this->model_settings.apply();
}

ModelSettingsBox::ModelSettingsBox(QWidget* parent)
	: AlignedGroupBox(parent, "Model Settings"),
	m_bExportMaterials(new QCheckBox("Export Materials", this)),
	m_bExportUvs(new QCheckBox("Export UV Coordinates", this)),
	m_bExportNormals(new QCheckBox("Export Normals", this))
{
	m_layout->addWidget(m_bExportMaterials);
	m_layout->addWidget(m_bExportUvs);
	m_layout->addWidget(m_bExportNormals);

	QObject::connect(m_bExportUvs, &QCheckBox::stateChanged, this,
		[this]() -> void {
			if (!m_bExportUvs->isChecked())
				m_bExportMaterials->setChecked(false);

			m_bExportMaterials->setEnabled(m_bExportUvs->isChecked());
		}
	);

	m_bExportMaterials->setChecked(SharedConverterSettings::ExportMaterials
		&& SharedConverterSettings::ExportUvs);
	m_bExportUvs->setChecked(SharedConverterSettings::ExportUvs);
	m_bExportNormals->setChecked(SharedConverterSettings::ExportNormals);
}

void ModelSettingsBox::fillSettings(ModelExportSettings* settings) const
{
	settings->export_materials = m_bExportMaterials->isChecked();
	settings->export_normals = m_bExportNormals->isChecked();
	settings->export_uvs = m_bExportUvs->isChecked();
}



class BlueprintNameValidator : public QValidator
{
public:
	BlueprintNameValidator(QWidget* parent = nullptr)
		: QValidator(parent) {}
	~BlueprintNameValidator() = default;

	QValidator::State validate(QString& str, int& pos) const override
	{
		for (QString::iterator v_iter = str.begin(); v_iter != str.end();)
		{
			if (!String::IsPathCharacterAllowed(v_iter->unicode()))
				v_iter = str.erase(v_iter);
			else
				v_iter++;
		}

		return QValidator::Acceptable;
	}
};

ConverterSettingsGuiBase::ConverterSettingsGuiBase(
	QWidget* parent,
	const std::wstring& object_name)
	: QDialog(parent),
	m_objectLayout(new QVBoxLayout(this)),
	m_objectName(this),
	m_convertButton(new QPushButton("Convert", this))
{
	this->setResult(DialogCode::Rejected);

	m_objectName.m_lineEdit->setValidator(
		new BlueprintNameValidator(m_objectName.m_lineEdit));
	m_objectName.m_lineEdit->setMaxLength(64);
	m_objectName.m_lineEdit->setText(
		QString::fromStdWString(object_name));

	m_convertButton->setEnabled(!object_name.empty());
	m_convertButton->setFixedHeight(m_convertButton->fontMetrics().height()
		+ g_windowPadding * 2);

	QObject::connect(m_objectName.m_lineEdit, &QLineEdit::textChanged, this,
		[this]() -> void {
			m_convertButton->setEnabled(!m_objectName.m_lineEdit->text().isEmpty());
		}
	);

	QObject::connect(m_convertButton, &QPushButton::pressed, this, &QDialog::accept);
}
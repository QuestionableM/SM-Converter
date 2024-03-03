#include "TileConverterSettingsGui.hpp"

#include "Converter/ConvertSettings.hpp"

///////////TILE EXPORT SETTINGS//////////////

void TileExportSettings::apply() const
{
	//Ground texture settings
	TileConverterSettings::Export8kGroundTextures = this->export_8k_ground_textures
		&& this->export_ground_textures;
	TileConverterSettings::ExportGroundTextures = this->export_ground_textures;

	//Tile entity settings
	TileConverterSettings::ExportHarvestables = this->export_harvestables;
	TileConverterSettings::ExportBlueprints = this->export_blueprints;
	TileConverterSettings::ExportKinematics = this->export_kinematics;
	TileConverterSettings::ExportClutter = this->export_clutter;
	TileConverterSettings::ExportPrefabs = this->export_prefabs;
	TileConverterSettings::ExportDecals = this->export_decals;
	TileConverterSettings::ExportAssets = this->export_assets;
}

//////////TILE CONVERTER THREAD DATA///////////

void TileConverterThreadData::applySettings() const
{
	this->applySettingsBase();
	this->export_settings.apply();
}

///////////TILE MODEL SETTINGS BOX/////////////

TileModelSettingsBox::TileModelSettingsBox(QWidget* parent)
	: ModelSettingsBox(parent),
	m_bExportGroundTextures(new QCheckBox("Export Ground Textures", this)),
	m_b8kGroundTextures(new QCheckBox("8K Ground Textures", this))
{
	m_layout->addWidget(m_bExportGroundTextures);
	m_layout->addWidget(m_b8kGroundTextures);

	QObject::connect(m_bExportGroundTextures, &QCheckBox::stateChanged, this,
		[this]() -> void {
			if (!m_bExportGroundTextures->isChecked())
				m_b8kGroundTextures->setChecked(false);

			m_b8kGroundTextures->setEnabled(m_bExportGroundTextures->isChecked());
		}
	);

	m_bExportGroundTextures->setChecked(TileConverterSettings::ExportGroundTextures);
	m_b8kGroundTextures->setChecked(TileConverterSettings::ExportGroundTextures
		&& TileConverterSettings::Export8kGroundTextures);
}

/////////////////TILE SETTINGS BOX//////////////////

TileSettingsBox::TileSettingsBox(QWidget* parent)
	: AlignedGroupBox(parent, "Tile Settings"),
	m_bExportClutter(new QCheckBox("Export Clutter", this)),
	m_bExportAssets(new QCheckBox("Export Assets", this)),
	m_bExportPrefabs(new QCheckBox("Export Prefabs", this)),
	m_bExportBlueprints(new QCheckBox("Export Blueprints", this)),
	m_bExportKinematics(new QCheckBox("Export Kinematics", this)),
	m_bExportHarvestables(new QCheckBox("Export Harvestables", this)),
	m_bExportDecals(new QCheckBox("Export Decals (WIP)", this))
{
	m_bExportClutter->setChecked(TileConverterSettings::ExportClutter);
	m_bExportAssets->setChecked(TileConverterSettings::ExportAssets);
	m_bExportPrefabs->setChecked(TileConverterSettings::ExportPrefabs);
	m_bExportBlueprints->setChecked(TileConverterSettings::ExportBlueprints);
	m_bExportKinematics->setChecked(TileConverterSettings::ExportKinematics);
	m_bExportHarvestables->setChecked(TileConverterSettings::ExportHarvestables);
	m_bExportDecals->setChecked(TileConverterSettings::ExportDecals);

	m_layout->addWidget(m_bExportClutter);
	m_layout->addWidget(m_bExportAssets);
	m_layout->addWidget(m_bExportPrefabs);
	m_layout->addWidget(m_bExportBlueprints);
	m_layout->addWidget(m_bExportKinematics);
	m_layout->addWidget(m_bExportHarvestables);
	m_layout->addWidget(m_bExportDecals);
}

//////////////TILE CONVERTER SETTINGS GUI////////////////

TileConverterSettingsGui::TileConverterSettingsGui(
	QWidget* parent,
	const std::wstring& filename,
	const std::wstring& path)
	: ConverterSettingsGuiBase(parent, filename),
	m_settingsLayout(new QHBoxLayout(this)),
	m_modelSettings(this),
	m_settings(this),
	m_customGameContent(this, path)
{
	this->setWindowFlags(this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	this->setWindowTitle("Tile Converter Settings");

	m_objectName.setTitle("Tile Name");

	m_settingsLayout->setContentsMargins(0, 0, 0, 0);
	m_settingsLayout->addWidget(&m_settings);
	m_settingsLayout->addWidget(&m_modelSettings);

	m_objectLayout->addWidget(&m_objectName, 0, Qt::AlignTop);
	m_objectLayout->addLayout(m_settingsLayout);
	m_objectLayout->addWidget(&m_customGameContent, 0, Qt::AlignTop);
	m_objectLayout->addWidget(m_convertButton, 0, Qt::AlignBottom);

	this->adjustSize();
	this->setMinimumSize(this->size());
}

void TileConverterSettingsGui::getThreadData(TileConverterThreadData* data)
{
	data->name = m_objectName.m_lineEdit->text().toStdWString();
	data->custom_game_idx.value = m_customGameContent.getCustomGameIndex();

	m_modelSettings.fillSettings(&data->model_settings);

	TileExportSettings& v_exp_settings = data->export_settings;
	v_exp_settings.export_8k_ground_textures = m_modelSettings.m_b8kGroundTextures->isChecked();
	v_exp_settings.export_ground_textures = m_modelSettings.m_bExportGroundTextures->isChecked();
	v_exp_settings.export_harvestables = m_settings.m_bExportHarvestables->isChecked();
	v_exp_settings.export_blueprints = m_settings.m_bExportBlueprints->isChecked();
	v_exp_settings.export_kinematics = m_settings.m_bExportKinematics->isChecked();
	v_exp_settings.export_clutter = m_settings.m_bExportClutter->isChecked();
	v_exp_settings.export_prefabs = m_settings.m_bExportPrefabs->isChecked();
	v_exp_settings.export_decals = m_settings.m_bExportDecals->isChecked();
	v_exp_settings.export_assets = m_settings.m_bExportAssets->isChecked();
}
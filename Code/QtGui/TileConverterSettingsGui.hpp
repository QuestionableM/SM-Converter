#pragma once

#include "ConverterSettingsGuiBase.hpp"
#include "CustomGameSelector.hpp"


struct TileExportSettings
{
public:
	void apply() const;

public:
	bool export_8k_ground_textures;
	bool export_ground_textures;
	bool export_harvestables;
	bool export_blueprints;
	bool export_kinematics;
	bool export_clutter;
	bool export_prefabs;
	bool export_decals;
	bool export_assets;
};

struct TileConverterThreadData : public ConverterThreadDataBase
{
public:
	void applySettings() const;

public:
	CustomGameIndex custom_game_idx;
	TileExportSettings export_settings;
};


class TileModelSettingsBox : public ModelSettingsBox
{
public:
	TileModelSettingsBox(QWidget* parent);
	~TileModelSettingsBox() = default;

	QCheckBox* m_bExportGroundTextures;
	QCheckBox* m_b8kGroundTextures;
};

class TileSettingsBox : public AlignedGroupBox
{
public:
	TileSettingsBox(QWidget* parent);
	~TileSettingsBox() = default;

	QCheckBox* m_bExportClutter;
	QCheckBox* m_bExportAssets;
	QCheckBox* m_bExportPrefabs;
	QCheckBox* m_bExportBlueprints;
	QCheckBox* m_bExportKinematics;
	QCheckBox* m_bExportHarvestables;
	QCheckBox* m_bExportDecals;
};

class TileConverterSettingsGui : public ConverterSettingsGuiBase
{
public:
	TileConverterSettingsGui(
		QWidget* parent,
		const std::wstring& filename,
		const std::wstring& path);
	~TileConverterSettingsGui() = default;

	void getThreadData(TileConverterThreadData* data);

	QWidget* m_settingsLayoutHolder;
	QHBoxLayout* m_settingsLayout;
	
	TileModelSettingsBox m_modelSettings;
	TileSettingsBox m_settings;
	CustomGameSelector m_customGameContent;
};
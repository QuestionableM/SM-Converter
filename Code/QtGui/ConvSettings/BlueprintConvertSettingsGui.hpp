#pragma once

#include "ConverterSettingsGuiBase.hpp"
#include "CustomGameSelector.hpp"

#include <QMainWindow>
#include <QPushButton>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QDialog>
#include <QLayout>

#include <string>

struct BlueprintConverterThreadData : public ConverterThreadDataBase
{
public:
	void applySettings() const;

public:
	CustomGameIndex custom_game_idx;
	int separation_type;
};

class BlueprintConverterSettingsGui : public ConverterSettingsGuiBase
{
public:
	BlueprintConverterSettingsGui(
		QWidget* parent,
		const std::wstring& filename,
		const std::wstring& path);
	~BlueprintConverterSettingsGui() = default;

	void getThreadData(BlueprintConverterThreadData* data);

private:
	NamedComboBox m_cbSeparationType;
	ModelSettingsBox m_modelSettings;
	CustomGameSelector m_cbCustomGameContent;
};
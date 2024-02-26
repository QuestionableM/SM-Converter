#pragma once

#include "AlignedGroupBox.hpp"

#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QLayout>
#include <QDialog>

#include <string>

struct ModelExportSettings
{
public:
	void apply() const;

public:
	bool export_materials;
	bool export_uvs;
	bool export_normals;
};

struct CustomGameIndex
{
public:
	class CustomGame* getPtr() const;

public:
	int value;
};

struct ConverterThreadDataBase
{
public:
	void applySettingsBase() const;

public:
	std::wstring name;
	std::wstring path;

	ModelExportSettings model_settings;
};

class ModelSettingsBox : public AlignedGroupBox
{
public:
	ModelSettingsBox(QWidget* parent);
	~ModelSettingsBox() = default;

	void fillSettings(ModelExportSettings* settings) const;

	QCheckBox* m_bExportMaterials;
	QCheckBox* m_bExportUvs;
	QCheckBox* m_bExportNormals;
};

class ConverterSettingsGuiBase : public QDialog
{
public:
	ConverterSettingsGuiBase(QWidget* parent, const std::wstring& object_name);
	~ConverterSettingsGuiBase() = default;

protected:
	QVBoxLayout* m_objectLayout;

	ObjectNameBox m_objectName;
	QPushButton* m_convertButton;
};
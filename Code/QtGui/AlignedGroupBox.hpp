#pragma once

#include <QComboBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QLayout>

class AlignedGroupBox : public QGroupBox
{
public:
	AlignedGroupBox(QWidget* parent);
	AlignedGroupBox(QWidget* parent, const QString& name);
	~AlignedGroupBox() = default;

	QVBoxLayout* m_layout;
};

class ObjectNameBox : public AlignedGroupBox
{
public:
	ObjectNameBox(QWidget* parent);
	~ObjectNameBox() = default;

	QLineEdit* m_lineEdit;
};

class NamedComboBox : public AlignedGroupBox
{
public:
	NamedComboBox(QWidget* parent = nullptr);
	~NamedComboBox() = default;

	QComboBox* m_comboBox;
};
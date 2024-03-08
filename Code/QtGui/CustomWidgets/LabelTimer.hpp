#pragma once

#include "CenteredLabel.hpp"

#include <QWidget>
#include <QLabel>
#include <QTimer>

class LabelTimer : public QLabel
{
public:
	LabelTimer(const QString& text, QWidget* parent = nullptr);
	LabelTimer(QWidget* parent = nullptr);
	~LabelTimer() = default;

public:
	QTimer* m_timer;
};

class CenteredLabelTimer : public CenteredLabel
{
public:
	CenteredLabelTimer(const QString& text, QWidget* lbl_parent = nullptr, QWidget* layout_parent = nullptr);
	CenteredLabelTimer(const QString& text, QWidget* parent = nullptr);
	CenteredLabelTimer(QWidget* parent = nullptr);
	~CenteredLabelTimer() = default;

public:
	QTimer* m_timer;
};
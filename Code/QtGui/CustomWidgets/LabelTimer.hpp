#pragma once

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
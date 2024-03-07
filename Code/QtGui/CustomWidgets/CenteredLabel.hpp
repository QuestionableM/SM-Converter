#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>

class CenteredLabel : public QLabel
{
public:
	CenteredLabel(const QString& text, QWidget* label_parent, QWidget* layout_parent);
	CenteredLabel(const QString& text, QWidget* parent = nullptr);
	CenteredLabel(QWidget* parent = nullptr);
	~CenteredLabel() = default;

private:
	void setup();

	QHBoxLayout* m_centerLayout;
};
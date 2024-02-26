#pragma once

#include <QDialog>
#include <QLayout>
#include <QLabel>

class RichTextLabel : public QLabel
{
public:
	RichTextLabel(const QString& text, QWidget* parent = nullptr);
	RichTextLabel(const QString& text, const QString& link, QWidget* parent = nullptr);
	~RichTextLabel() = default;
};

class AboutProgramGui : public QDialog
{
public:
	AboutProgramGui(QWidget* parent = nullptr);
	~AboutProgramGui() = default;

private:
	QLabel* m_description;

	QVBoxLayout* m_mainLayout;
	QHBoxLayout* m_subLayout;

	//Authors panel
	QVBoxLayout* m_authorsPanelLayout;
	QLabel* m_versionLabel;
	QLabel* m_authorsLabel;
	RichTextLabel* m_author1Label;
	RichTextLabel* m_author2Label;

	//Libraries panel
	QVBoxLayout* m_librariesLayout;
};
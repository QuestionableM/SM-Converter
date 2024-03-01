#pragma once

#include "Gui/SettingsGuiChangeDetector.hpp"
#include "PathListViewWidget.hpp"
#include "AlignedGroupBox.hpp"

#include <QPushButton>
#include <QTabWidget>
#include <QCheckBox>
#include <QDialog>
#include <QLayout>

#include <unordered_set>

class PathGroupBox : public AlignedGroupBox
{
public:
	PathGroupBox(const QString& title, QWidget* parent = nullptr);
	~PathGroupBox() = default;

	QLineEdit* m_path;
	QPushButton* m_selectPathButton;

	QHBoxLayout* m_hLayout;
};

class SettingsGeneralTab : public QWidget
{
public:
	SettingsGeneralTab(QWidget* parent = nullptr);
	~SettingsGeneralTab() = default;

	PathGroupBox* m_pathGroupBox;
	QCheckBox* m_bOpenLinksInSteam;

	QVBoxLayout* m_mainLayout;
};

class SettingsPathsTab : public QWidget
{
public:
	SettingsPathsTab(QWidget* parent = nullptr);
	~SettingsPathsTab() = default;

	template<typename T>
	void updatePathListView(const T& list)
	{
		m_pathListView->clearItemsSilent();

		for (const std::wstring& v_cur_str : list)
			m_pathListView->addItemSilent(QString::fromStdWString(v_cur_str));

		m_pathListView->updateScrollBar();
		m_pathListView->repaint();
	}

	void updateCurrentPathList();

public:
	SettingsChangeDetector m_changeDetector;

	PathListViewWidget* m_pathListView;

	QComboBox* m_folderOptions;
	QVBoxLayout* m_mainLayout;
};

class ProgramSettingsGui : public QDialog
{
public:
	ProgramSettingsGui(QWidget* parent = nullptr);
	~ProgramSettingsGui() = default;

private:
	void keyPressEvent(QKeyEvent* event) override;

public:
	QTabWidget* m_tabBar;
	QPushButton* m_saveSettingsBtn;

	QVBoxLayout* m_layout;
};
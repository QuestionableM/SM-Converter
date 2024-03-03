#pragma once

#include "QtGui/CustomWidgets/PathListViewWidget.hpp"
#include "QtGui/CustomWidgets/AlignedGroupBox.hpp"
#include "Gui/SettingsGuiChangeDetector.hpp"

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

class SettingsTabBase : public QWidget
{
	Q_OBJECT

public:
	inline SettingsTabBase(QWidget* parent = nullptr)
		: QWidget(parent) {}
	~SettingsTabBase() = default;

signals:
	void onSettingChanged();
};

class SettingsGeneralTab : public SettingsTabBase
{
public:
	SettingsGeneralTab(
		SettingsChangeDetector& detector,
		QWidget* parent = nullptr);
	~SettingsGeneralTab() = default;

	void onCheckboxUpdate();
	void onGamePathUpdate();

public:
	SettingsChangeDetector& m_changeDetector;

	PathGroupBox* m_pathGroupBox;
	QCheckBox* m_bOpenLinksInSteam;

	QVBoxLayout* m_mainLayout;
};

class SettingsPathsTab : public SettingsTabBase
{
public:
	SettingsPathsTab(
		SettingsChangeDetector& detector,
		QWidget* parent = nullptr);
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

	bool onElementAdded(int idx, const QString& str);
	bool onElementRemove(int idx);

public:
	SettingsChangeDetector& m_changeDetector;
	PathListViewWidget* m_pathListView;

	QComboBox* m_folderOptions;
	QVBoxLayout* m_mainLayout;
};

class ProgramSettingsGui : public QDialog
{
public:
	ProgramSettingsGui(QWidget* parent = nullptr);
	~ProgramSettingsGui() = default;

	void updateSaveButton();
	void saveButtonClick();

	bool shouldReloadObjectDatabase() const;
	bool shouldReloadUserObjects() const;

private:
	void keyPressEvent(QKeyEvent* event) override;
	void closeEvent(QCloseEvent* event) override;

private:
	SettingsChangeDetector m_changeDetector;

	SettingsGeneralTab* m_generalTab;
	SettingsPathsTab* m_pathsTab;

	QTabWidget* m_tabBar;
	QPushButton* m_saveSettingsBtn;

	QVBoxLayout* m_layout;

	bool m_bReloadObjectDatabase;
	bool m_bReloadUserObjects;
};
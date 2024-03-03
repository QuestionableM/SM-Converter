#pragma once

#include <QMenuBar>
#include <QWidget>

class MainGuiMenuBar : public QMenuBar
{
public:
	MainGuiMenuBar(QWidget* parent = nullptr);
	~MainGuiMenuBar() = default;

public:
	//Open menu actions
	QAction* m_openBlueprintOutDirAction;
	QAction* m_openWorldOutDirAction;
	QAction* m_openTileOutDirAction;
	QAction* m_aboutProgramAction;
	QAction* m_aboutQtAction;

	//Settings menu actions
	QAction* m_reloadUserObjectsAction;
	QAction* m_reloadObjectDatabaseAction;
	QAction* m_openProgramSettingsAction;
};
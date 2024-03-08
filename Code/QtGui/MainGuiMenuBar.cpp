#include "MainGuiMenuBar.hpp"

#include <QApplication>

MainGuiMenuBar::MainGuiMenuBar(QWidget* parent)
	: QMenuBar(parent),
	m_openBlueprintOutDirAction(new QAction("Blueprint Output directory", this)),
	m_openWorldOutDirAction(new QAction("World Output Directory", this)),
	m_openTileOutDirAction(new QAction("Tile Output Directory", this)),
	m_aboutProgramAction(new QAction("About Program", this)),
	m_aboutQtAction(new QAction("About QT", this)),
	m_reloadUserObjectsAction(new QAction("Reload User Objects", this)),
	m_reloadObjectDatabaseAction(new QAction("Reload Object Database", this)),
	m_openProgramSettingsAction(new QAction("Settings", this))
{
	QObject::connect(m_aboutQtAction, &QAction::triggered, QApplication::aboutQt);

	QMenu* v_openMenu = this->addMenu("Open");
	v_openMenu->addAction(m_openBlueprintOutDirAction);
	v_openMenu->addAction(m_openWorldOutDirAction);
	v_openMenu->addAction(m_openTileOutDirAction);
	v_openMenu->addSeparator();
	v_openMenu->addAction(m_aboutProgramAction);
	v_openMenu->addSeparator();
	v_openMenu->addAction(m_aboutQtAction);

	QMenu* v_settingsMenu = this->addMenu("Settings");
	v_settingsMenu->addAction(m_reloadUserObjectsAction);
	v_settingsMenu->addAction(m_reloadObjectDatabaseAction);
	v_settingsMenu->addSeparator();
	v_settingsMenu->addAction(m_openProgramSettingsAction);
}
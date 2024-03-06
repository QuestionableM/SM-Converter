#include "MainGuiContextMenu.hpp"

ObjectListContextMenu::ObjectListContextMenu(QWidget* parent)
	: QMenu(parent),
	m_openInSteamWorkshopAction(new QAction("Open in Steam Workshop", this)),
	m_openInExplorerAction(new QAction("Open in File Explorer", this)),
	m_openCreatorProfile(new QAction("Open Creator Profile", this)),
	m_copyWorkshopIdAction(new QAction("Copy Workshop Id", this)),
	m_copyCreatorIdAction(new QAction("Copy Creator Id", this)),
	m_copyUuidAction(new QAction("Copy UUID", this)),
	m_showObjectInfoAction(new QAction("Show Object Info", this))
{
	this->addAction(m_openInSteamWorkshopAction);
	this->addAction(m_openInExplorerAction);
	this->addAction(m_openCreatorProfile);
	this->addSeparator();
	this->addAction(m_copyWorkshopIdAction);
	this->addAction(m_copyCreatorIdAction);
	this->addAction(m_copyUuidAction);
	this->addSeparator();
	this->addAction(m_showObjectInfoAction);
}
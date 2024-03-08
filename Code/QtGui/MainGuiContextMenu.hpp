#pragma once

#include <QMenu>

class ObjectListContextMenu : public QMenu
{
public:
	ObjectListContextMenu(QWidget* parent = nullptr);
	~ObjectListContextMenu() = default;

public:
	QAction* m_openInSteamWorkshopAction;
	QAction* m_openInExplorerAction;
	QAction* m_openCreatorProfile;
	QAction* m_copyWorkshopIdAction;
	QAction* m_copyCreatorIdAction;
	QAction* m_copyUuidAction;
	QAction* m_showObjectInfoAction;
};
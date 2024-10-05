#pragma once

#include "ObjectDatabase/UserDataReaders/FilterSettingsData.hpp"
#include "QtGui/CustomWidgets/CenteredLabel.hpp"
#include "QtGui/CustomWidgets/ImageBox.hpp"

#include <QListWidget>
#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QImage>
#include <QMenu>

#include <string>

class ModListContextMenu : public QMenu
{
public:
	ModListContextMenu(QWidget* parent = nullptr);
	~ModListContextMenu() = default;

	QAction* m_openInSteamWorkshopAction;
	QAction* m_openInExplorerAction;
};

class ModListWidget : public QListWidget
{
public:
	ModListWidget(QWidget* parent = nullptr);
	~ModListWidget() = default;

	class SMMod* getSelectedMod() const;
	void updateContextMenu();
	void updateModList();


	void openModInWorkshop();
	void openModInExplorer();

private:
	void contextMenuEvent(QContextMenuEvent* event) override;

private:
	ModListContextMenu* m_contextMenu;
	CenteredLabel* m_noModsLabel;
};

class ObjectInfoGui : public QDialog
{
public:
	ObjectInfoGui(const QString& title, const std::wstring& image, QWidget* parent = nullptr);
	ObjectInfoGui(struct BlueprintInstance* bprint, QWidget* parent = nullptr);
	ObjectInfoGui(struct WorldInstance* world, QWidget* parent = nullptr);
	ObjectInfoGui(struct TileInstance* tile, QWidget* parent = nullptr);
	~ObjectInfoGui() = default;

private:
	void appendPartsStats();

	template<typename T>
	inline void appendMainObjectInfo(T* obj)
	{
		const QString v_bp_name = QString("Name: %1")
			.arg(QString::fromStdWString(obj->name));
		const QString v_uuid = QString("UUID: %1")
			.arg(QString::fromStdString(obj->uuid.ToString()));
		const QString v_content_type = QString("Content Type: %1")
			.arg(QString::fromStdWString(FilterSettingsData::GetFilterName(obj->filter)));

		m_infoLayout->addWidget(new QLabel(v_bp_name, this));
		m_infoLayout->addWidget(new QLabel(v_uuid, this));
		m_infoLayout->addWidget(new QLabel(v_content_type, this));
	}

private:
	QVBoxLayout* m_mainLayout;

	//Left: Image, Right: Info
	QHBoxLayout* m_mainInfoLayout;
	ImageBox* m_objectImage;
	//Handles all the label data about the object
	QVBoxLayout* m_infoLayout;
	ModListWidget* m_modList;
};
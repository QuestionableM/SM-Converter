#pragma once

#include <QScrollBar>
#include <QWidget>
#include <QEvent>
#include <QList>

#include <functional>

class UserObjectItem
{
public:
	UserObjectItem(const QString& name, const QString& descr)
		: name(name), description(descr) {}

	UserObjectItem(const QString& name, const QString& descr, const QString& icon_path = "")
		: name(name), description(descr), icon_path(icon_path) {}

	QString name;
	QString description;
	QString icon_path;
};

class UserObjectListView : public QWidget
{
	Q_OBJECT

public:
	UserObjectListView(QWidget* parent = nullptr);
	~UserObjectListView();

	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void contextMenuEvent(QContextMenuEvent* event) override;

	void setIndexFromMousePosition(const QPoint& pos);
	bool isItemVisible(int idx) const;
	void setIndex(int idx);
	void setContextMenuStrip(QMenu* menu);

	bool hasSelection() const;
	void clearSelection();

	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

	//Adds a new object without updating the view
	void addNewObjectSilent(const UserObjectItem& new_item);
	//Updates the view with each call to this function
	void addNewObject(const UserObjectItem& new_item);
	
	QImage& loadImageCached(const QString& path);
	void clearImageCache();

	void silentClear();
	void clear();

	void updateScrollbar();
	void updateGraphics();

	int sliderPosition() const;
	int selectedIndex() const { return m_selectedIdx; }

signals:
	void selectedIndexChanged();

public:
	int m_selectedIdx;

	QList<UserObjectItem> m_objectStorage;
	QHash<QString, QImage> m_imageCache;
	QScrollBar* m_scrollBar;

	QMenu* m_contextMenuStrip;
};
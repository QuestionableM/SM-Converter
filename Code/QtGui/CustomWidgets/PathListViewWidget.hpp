#pragma once

#include <QPushButton>
#include <QScrollBar>
#include <QLineEdit>
#include <QWidget>
#include <QLayout>
#include <QEvent>
#include <QMenu>

#include <functional>

class PathEditorWidget : public QWidget
{
	Q_OBJECT

public:
	PathEditorWidget(QWidget* parent = nullptr);
	~PathEditorWidget() = default;

	bool isWidgetOwned(QWidget* widget) const;
	void open(int line_idx, const QString& current_path);
	void finish(bool apply_string);

	bool invokeInputFilter();
	void setInputFilter(const std::function<bool(int, const QString&)>& filter);
	int currentIndex() const;
	QString text() const;

	void keyPressEvent(QKeyEvent* event) override;

private slots:
	void onFocusChanged(QWidget* old, QWidget* now);

signals:
	void onStringApplied();

private:
	std::function<bool(int, const QString&)> m_inputFilter;

	QString m_originalText;
	int m_currentIdx;

	QLineEdit* m_path;
	QPushButton* m_selectPathButton;

	QHBoxLayout* m_layout;
};

class PathListViewContextMenu : public QMenu
{
public:
	PathListViewContextMenu(QWidget* parent = nullptr);
	~PathListViewContextMenu() = default;

public:
	QAction* m_addNewElementAction;
	QAction* m_editElementAction;
	QAction* m_removeElementAction;
};

class PathListViewWidget : public QWidget
{
public:
	PathListViewWidget(QWidget* parent = nullptr);
	~PathListViewWidget() = default;

	int getVisibleLineCount() const;
	//Adjusts the amount of visible lines depending on coverage rate of the last item
	int getTrulyVisibleLineCount(int coverage = 80) const;
	//Calculates the percentage of visible area for a specific item
	int calculateItemVisibleArea(int idx) const;
	void selectItemFromMousePosition(QPoint pos);

	void updateContextMenu();
	void updateScrollBar();
	int sliderPosition() const;
	int itemWidth() const;
	bool isIdxValid(int idx) const;
	bool isIdxValidStrict(int idx) const;
	//If the index is not visible, the slider will move to that specific element
	void moveSliderToIndex(int idx);
	void moveCurrentIndex(bool is_up);
	void openEditor(int item_idx);

	bool removeElement(int idx);
	void removeSelectedElement();
	void clearItemsSilent();

	void addItemSilent(const QString& str);

	void setRemoveElementCallback(const std::function<bool(int)>& callback);
	void setAddElementCallback(const std::function<bool(int, const QString&)>& callback);

	bool invokeRemoveCallback(int idx) const;

public:
	std::function<bool(int)> m_removeCallback;

	PathListViewContextMenu m_contextMenu;
	PathEditorWidget* m_editor;
	QScrollBar* m_scrollBar;
	QStringList m_pathStorage;
	int m_currentIdx;
	int m_itemHeight;

private:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

	void wheelEvent(QWheelEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	void contextMenuEvent(QContextMenuEvent* event) override;

	void keyPressEvent(QKeyEvent* event) override;
	void applyEditorString();
};
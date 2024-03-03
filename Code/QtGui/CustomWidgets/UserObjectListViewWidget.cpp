#include "UserObjectListViewWidget.hpp"

#include "Utils/Console.hpp"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QMenu>

UserObjectListView::UserObjectListView(QWidget* parent)
	: QWidget(parent)
{
	this->setFocusPolicy(Qt::FocusPolicy::StrongFocus);

	m_scrollBar = new QScrollBar(Qt::Orientation::Vertical, this);
	m_scrollBar->setFocusProxy(this);
	m_scrollBar->setRange(0, 0);
	m_scrollBar->setValue(0);
	connect(m_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(repaint(void)));

	m_selectedIdx = -1;
	m_contextMenuStrip = nullptr;

	this->updateGraphics();
}

UserObjectListView::~UserObjectListView()
{
	delete m_scrollBar;
}

constexpr int g_singleItemHeight = 75;
constexpr int g_itemMargin = 10;
constexpr int g_itemImageSize = g_singleItemHeight - (g_itemMargin * 2);
constexpr int g_itemTextRegionX = g_singleItemHeight;

void UserObjectListView::paintEvent(QPaintEvent* event)
{
	const QPalette v_palette = QApplication::palette();
	QPainter v_painter(this);
	v_painter.setRenderHint(QPainter::RenderHint::TextAntialiasing);
	v_painter.setRenderHint(QPainter::RenderHint::Antialiasing);

	v_painter.fillRect(this->rect(), v_palette.color(QPalette::ColorRole::Base));

	const int v_num_vis_objects = (this->height() / g_singleItemHeight) + 1;
	const int v_offset = this->sliderPosition();
	const int v_rem_lines = std::min(std::max(int(m_objectStorage.size()) - v_offset, 0), v_num_vis_objects);
	const int v_item_width = this->width() - (m_scrollBar->isVisible() ? m_scrollBar->width() : 0);

	const bool v_is_enabled = this->isEnabled();
	const QPalette::ColorGroup v_col_group = v_is_enabled
		? QPalette::ColorGroup::Current
		: QPalette::ColorGroup::Disabled;

	const QTextOption v_text_opt(Qt::AlignVCenter);
	const QColor v_text_color = v_palette.color(v_col_group, QPalette::ColorRole::Text);
	const QColor v_sel_color = v_palette.color(QPalette::ColorRole::Highlight);

	QPen v_pen(v_palette.color(v_col_group, QPalette::ColorRole::Dark));
	v_pen.setCosmetic(true);
	v_pen.setWidth(0);

	QFont v_title_font(this->font());
	v_title_font.setBold(true);

	for (int a = 0; a < v_rem_lines; a++)
	{
		const int v_cur_idx = a + v_offset;
		const int v_line_height = g_singleItemHeight * a;
		const UserObjectItem& v_cur_item = m_objectStorage[v_cur_idx];

		if (v_cur_idx == m_selectedIdx)
		{
			v_painter.fillRect(0, v_line_height, v_item_width, g_singleItemHeight,
				v_sel_color);
		}

		const QRect v_img_rect(
			g_itemMargin,
			g_itemMargin + v_line_height,
			g_itemImageSize,
			g_itemImageSize);

		v_painter.setOpacity(v_is_enabled ? 1.0f : 0.5f);
		v_painter.drawImage(v_img_rect, this->loadImageCached(v_cur_item.icon_path));
		v_painter.setOpacity(1.0f);

		v_painter.setPen(v_pen);
		v_painter.drawRect(v_img_rect);

		v_painter.setPen(v_text_color);
		const QRect v_title_rect(g_itemTextRegionX, v_line_height + 10, v_item_width, QFontMetrics(v_title_font).height());
		const QRect v_desc_rect(g_itemTextRegionX, v_title_rect.top() + v_title_rect.height(), v_item_width, this->fontMetrics().height());
		
		v_painter.setFont(v_title_font);
		v_painter.drawText(v_title_rect, v_cur_item.name);

		v_painter.setFont(this->font());
		v_painter.drawText(v_desc_rect, v_cur_item.description);
	}

	v_painter.setPen(v_pen);
	v_painter.drawRect(this->rect());
}

void UserObjectListView::resizeEvent(QResizeEvent* event)
{
	this->updateGraphics();
}

void UserObjectListView::contextMenuEvent(QContextMenuEvent* event)
{
	if (m_contextMenuStrip)
		m_contextMenuStrip->exec(event->globalPos());
}

void UserObjectListView::setIndexFromMousePosition(const QPoint& pos)
{
	const int v_idx = (pos.y() / g_singleItemHeight) + this->sliderPosition();

	const int v_storage_sz = int(m_objectStorage.size());
	if (v_idx >= v_storage_sz || v_idx < 0)
		return;

	if (m_selectedIdx != v_idx)
	{
		m_selectedIdx = v_idx;
		this->selectedIndexChanged();
		this->repaint();
	}
}

bool UserObjectListView::isItemVisible(int idx) const
{
	const int v_num_visible_lines = (this->height() / g_singleItemHeight) + 1;
	const int v_range_start = this->sliderPosition();
	const int v_range_end = v_range_start + v_num_visible_lines;

	return (idx >= v_range_start && idx < v_range_end);
}

void UserObjectListView::setIndex(int idx)
{
	if (idx == m_selectedIdx)
		return;

	const int v_old_idx = m_selectedIdx;
	m_selectedIdx = idx;

	if (this->isItemVisible(v_old_idx) || this->isItemVisible(idx))
		this->repaint();
}

void UserObjectListView::setContextMenuStrip(QMenu* menu)
{
	m_contextMenuStrip = menu;
}

bool UserObjectListView::hasSelection() const
{
	return m_selectedIdx >= 0;
}

void UserObjectListView::clearSelection()
{
	this->setIndex(-1);
}

void UserObjectListView::mousePressEvent(QMouseEvent* event)
{
	this->setIndexFromMousePosition(event->localPos().toPoint());
}

void UserObjectListView::mouseMoveEvent(QMouseEvent* event)
{
	this->setIndexFromMousePosition(event->localPos().toPoint());
}

void UserObjectListView::wheelEvent(QWheelEvent* event)
{
	if (m_scrollBar->isVisible())
		QCoreApplication::sendEvent(m_scrollBar, event);
}

void UserObjectListView::addNewObjectSilent(const UserObjectItem& new_item)
{
	m_objectStorage.push_back(new_item);
}

void UserObjectListView::addNewObject(const UserObjectItem& new_item)
{
	this->addNewObjectSilent(new_item);
	this->updateScrollbar();
}

QImage& UserObjectListView::loadImageCached(const QString& path)
{
	auto v_iter = m_imageCache.find(path);
	if (v_iter != m_imageCache.end())
		return v_iter.value();

	return m_imageCache.emplace(path, QImage(path)).value();
}

void UserObjectListView::clearImageCache()
{
	m_imageCache.clear();
}

void UserObjectListView::silentClear()
{
	m_scrollBar->blockSignals(true);
	m_scrollBar->setValue(0);
	m_scrollBar->blockSignals(false);

	m_selectedIdx = -1;
	m_objectStorage.clear();
}

void UserObjectListView::clear()
{
	this->silentClear();
	this->updateGraphics();
}

void UserObjectListView::updateScrollbar()
{
	const int v_lines_on_screen = this->height() / g_singleItemHeight;
	const bool v_is_visible = v_lines_on_screen < m_objectStorage.size();
	
	m_scrollBar->setVisible(v_is_visible);

	if (v_is_visible)
	{
		m_scrollBar->setRange(0, m_objectStorage.size() - v_lines_on_screen);

		m_scrollBar->resize(15, this->height() - 2);
		m_scrollBar->move(this->width() - m_scrollBar->width() - 1, 1);
	}
}

void UserObjectListView::updateGraphics()
{
	this->updateScrollbar();
	this->repaint();
}

int UserObjectListView::sliderPosition() const
{
	return m_scrollBar->isVisible() ? m_scrollBar->sliderPosition() : 0;
}
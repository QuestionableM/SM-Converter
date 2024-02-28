#include "PathListViewWidget.hpp"

#include "QtUtil.hpp"

#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>

#include "Utils/Console.hpp"

PathEditorWidget::PathEditorWidget(QWidget* parent)
	: QWidget(parent),
	m_currentIdx(-1),
	m_path(new QLineEdit(this)),
	m_selectPathButton(new QPushButton("...", this)),
	m_layout(new QHBoxLayout(this))
{
	QObject::connect(m_selectPathButton, &QPushButton::pressed, this,
		[this]() -> void {
			this->blockSignals(true);

			QString v_selected_directory;
			if (QtUtil::fileDialog(this, "Select a directory",
				QFileDialog::FileMode::Directory, v_selected_directory))
			{
				m_path->setText(v_selected_directory);
			}

			this->blockSignals(false);
			m_path->setFocus();
		}
	);

	QObject::connect(
		qApp, SIGNAL(focusChanged(QWidget*, QWidget*)),
		this, SLOT(onFocusChanged(QWidget*, QWidget*)));
	m_path->setTextMargins(10, 0, 0, 0);

	m_layout->setContentsMargins(0, 0, 0, 0);
	m_layout->setSpacing(0);

	m_layout->addWidget(m_path);
	m_layout->addWidget(m_selectPathButton);

	this->setVisible(false);
}

bool PathEditorWidget::isWidgetOwned(QWidget* widget) const
{
	return widget == this || widget == m_path || widget == m_selectPathButton;
}

void PathEditorWidget::open(int line_idx, const QString& current_path)
{
	if (this->isVisible())
		return;

	m_path->setText(current_path);
	m_currentIdx = line_idx;

	this->setVisible(true);
	m_path->setFocus();
	m_path->selectAll();
}

void PathEditorWidget::finish(bool apply_string)
{
	if (!this->invokeInputFilter())
		return;

	if (apply_string)
		this->onStringApplied();

	this->setVisible(false);

	QWidget* v_parent = this->parentWidget();
	if (v_parent)
		v_parent->setFocus();
}

bool PathEditorWidget::invokeInputFilter()
{
	if (!m_inputFilter)
		return true;

	return m_inputFilter(m_path->text());
}

void PathEditorWidget::setInputFilter(const std::function<bool(const QString&)>& filter)
{
	m_inputFilter = filter;
}

int PathEditorWidget::currentIndex() const
{
	return m_currentIdx;
}

QString PathEditorWidget::text() const
{
	return m_path->text();
}

void PathEditorWidget::keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
	case Qt::Key::Key_Escape:
		this->finish(false);
		break;
	case Qt::Key::Key_Return:
		this->finish(true);
		break;
	default:
		QWidget::keyPressEvent(event);
		break;
	}
}

void PathEditorWidget::onFocusChanged(QWidget* old, QWidget* now)
{
	if (this->signalsBlocked())
		return;

	const bool v_focus_old = this->isWidgetOwned(old);
	const bool v_focus_now = this->isWidgetOwned(now);

	if (v_focus_old && !v_focus_now)
		this->setVisible(false);
}

///////////////////////PATH LIST VIEW WIDGET///////////////////////

PathListViewWidget::PathListViewWidget(QWidget* parent)
	: QWidget(parent),
	m_editor(new PathEditorWidget(this)),
	m_scrollBar(new QScrollBar(Qt::Vertical, this))
{
	QSizePolicy v_policy;
	v_policy.setControlType(QSizePolicy::ControlType::DefaultType);
	v_policy.setVerticalPolicy(QSizePolicy::Policy::Expanding);
	v_policy.setHorizontalPolicy(QSizePolicy::Policy::Expanding);
	v_policy.setVerticalStretch(1);
	v_policy.setHorizontalStretch(1);
	this->setSizePolicy(v_policy);

	this->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
	this->setContentsMargins(10, 10, 10, 10);

	m_scrollBar->setFocusProxy(this);
	m_scrollBar->setRange(0, 0);
	m_scrollBar->setSliderPosition(0);
	QObject::connect(m_scrollBar, SIGNAL(valueChanged(int)), this, SLOT(repaint(void)));

	m_currentIdx = -1;
	m_itemHeight = this->fontMetrics().height() + 10;
	m_editor->setFixedHeight(m_itemHeight);
	QObject::connect(
		m_editor, &PathEditorWidget::onStringApplied,
		this, &PathListViewWidget::applyEditorString);
}

int PathListViewWidget::getVisibleLineCount() const
{
	return this->height() / m_itemHeight;
}

int PathListViewWidget::getTrulyVisibleLineCount(int coverage) const
{
	const int v_line_count = this->getVisibleLineCount();
	const int v_line_to_remove =
		int(this->calculateItemVisibleArea(this->sliderPosition() + v_line_count) <= coverage);

	return std::max(v_line_count - v_line_to_remove, 0);
}

void PathListViewWidget::moveSliderToIndex(int idx)
{
	const int v_vision_start = this->sliderPosition();
	const int v_vision_end = v_vision_start + this->getTrulyVisibleLineCount();

	const bool v_is_visible = v_vision_start <= idx
		&& idx <= v_vision_end;

	if (v_is_visible)
		return;

	const int v_down_force = std::max(v_vision_start - idx, 0);
	const int v_up_force = std::max(idx - v_vision_end, 0);
	const int v_final_force = -v_down_force + v_up_force;

	m_scrollBar->setSliderPosition(m_scrollBar->sliderPosition() + v_final_force);
}

void PathListViewWidget::selectItemFromMousePosition(QPoint pos)
{
	const int v_cur_idx = (pos.y() / m_itemHeight) + this->sliderPosition();

	if (v_cur_idx < 0 || v_cur_idx >= m_pathStorage.size())
		return;

	if (v_cur_idx != m_currentIdx)
	{
		m_currentIdx = v_cur_idx;
		this->repaint();
	}
}

void PathListViewWidget::updateScrollBar()
{
	const int v_lines_on_screen = this->getVisibleLineCount();
	const bool v_is_visible = v_lines_on_screen < m_pathStorage.size();

	m_scrollBar->setVisible(v_is_visible);

	if (v_is_visible)
	{
		m_scrollBar->setRange(0, m_pathStorage.size() - v_lines_on_screen);

		m_scrollBar->resize(15, this->height() - 2);
		m_scrollBar->move(this->width() - m_scrollBar->width() - 1, 1);
	}
}

int PathListViewWidget::sliderPosition() const
{
	return m_scrollBar->isVisible() ? m_scrollBar->sliderPosition() : 0;
}

int PathListViewWidget::itemWidth() const
{
	return this->width() - (m_scrollBar->isVisible() ? m_scrollBar->width() : 0);
}

bool PathListViewWidget::isIndexValid(int idx) const
{
	return idx >= 0 && idx < m_pathStorage.size();
}

int PathListViewWidget::calculateItemVisibleArea(int idx) const
{
	const int v_pixels_start = this->sliderPosition() * m_itemHeight;
	const int v_pixels_end = v_pixels_start + this->height();

	const int v_item_start = idx * m_itemHeight;
	const int v_item_end = v_item_start + m_itemHeight;

	const int v_inter_start = std::max(v_item_start, v_pixels_start);
	const int v_inter_end = std::min(v_item_end, v_pixels_end);
	const int v_inter_area = std::max(v_inter_end - v_inter_start, 0);

	return int((float(v_inter_area) / float(m_itemHeight)) * 100.0f);
}

void PathListViewWidget::moveCurrentIndex(bool is_up)
{
	m_scrollBar->blockSignals(true);

	const int v_old_idx = m_currentIdx;

	if (is_up)
	{
		if (m_currentIdx > 0)
			m_currentIdx--;
	}
	else
	{
		if ((m_currentIdx + 1) < m_pathStorage.size())
			m_currentIdx++;
	}

	if (v_old_idx != m_currentIdx)
	{
		this->moveSliderToIndex(m_currentIdx);
		this->repaint();
	}

	m_scrollBar->blockSignals(false);
}

void PathListViewWidget::openEditor(int item_idx)
{
	if (item_idx != m_pathStorage.size() && !this->isIndexValid(item_idx))
		return;

	this->moveSliderToIndex(item_idx);
	const int v_local_idx = item_idx - this->sliderPosition();

	QString v_edit_string;
	//Allow to append new elements
	if (item_idx != m_pathStorage.size())
	{
		if (!this->isIndexValid(item_idx))
			return;

		v_edit_string = m_pathStorage[item_idx];
	}
	else
	{
		m_currentIdx = -1;
	}

	m_editor->setGeometry(
		0, v_local_idx * m_itemHeight,
		this->itemWidth(), m_itemHeight);
	m_editor->open(item_idx, v_edit_string);
}

void PathListViewWidget::removeElement(int idx)
{
	if (!this->isIndexValid(idx))
		return;

	this->moveSliderToIndex(idx);
	m_pathStorage.erase(m_pathStorage.begin() + idx);

	this->updateScrollBar();
}

void PathListViewWidget::removeSelectedElement()
{
	if (m_currentIdx == -1 || m_pathStorage.isEmpty())
		return;
	
	this->removeElement(m_currentIdx);
	m_currentIdx = std::clamp(m_currentIdx, 0,
		std::max((int)m_pathStorage.size() - 1, 0));

	this->repaint();
}

void PathListViewWidget::paintEvent(QPaintEvent* event)
{
	QPainter v_painter(this);

	const QPalette v_palette = QApplication::palette();
	const bool v_is_enabled = this->isEnabled();
	const QPalette::ColorGroup v_col_group = v_is_enabled
		? QPalette::ColorGroup::Current
		: QPalette::ColorGroup::Disabled;

	const QColor v_sel_color = v_palette.color(QPalette::ColorRole::Highlight);
	const QMargins v_margins = this->contentsMargins();

	const int v_offset = this->sliderPosition();
	const int v_item_width = this->itemWidth();
	const int v_num_vis_objects = this->getVisibleLineCount() + 1;
	const int v_rem_lines = std::min(std::max(int(m_pathStorage.size()) - v_offset, 0), v_num_vis_objects);

	QTextOption v_text_option(Qt::AlignVCenter);
	v_text_option.setWrapMode(QTextOption::NoWrap);

	for (std::size_t a = 0; a < v_rem_lines; a++)
	{
		const int v_cur_idx = a + v_offset;

		const QString& v_cur_path = m_pathStorage[v_cur_idx];
		const int v_cur_item_height = a * m_itemHeight;

		if (v_cur_idx == m_currentIdx)
		{
			if (m_editor->isVisible())
				continue;

			v_painter.fillRect(0, v_cur_item_height, v_item_width, m_itemHeight,
				v_sel_color);
		}

		const QRect v_text_rect(
			v_margins.left(),
			v_cur_item_height,
			v_item_width - v_margins.left(),
			m_itemHeight);

		v_painter.drawText(v_text_rect, v_cur_path, v_text_option);
	}

	v_painter.setPen(v_palette.color(v_col_group, QPalette::ColorRole::Dark));
	v_painter.drawRect(this->rect());
}

void PathListViewWidget::resizeEvent(QResizeEvent* event)
{
	this->updateScrollBar();

	if (m_editor->isVisible())
		m_editor->setVisible(false);
}

void PathListViewWidget::wheelEvent(QWheelEvent* event)
{
	if (m_scrollBar->isVisible() && !m_editor->isVisible())
		QCoreApplication::sendEvent(m_scrollBar, event);
}

void PathListViewWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (m_editor->isVisible())
		return;

	this->selectItemFromMousePosition(event->pos());
}

void PathListViewWidget::mousePressEvent(QMouseEvent* event)
{
	this->selectItemFromMousePosition(event->pos());
}

void PathListViewWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	const int v_global_idx = (event->pos().y() / m_itemHeight) + this->sliderPosition();
	this->openEditor(v_global_idx);
}

void PathListViewWidget::keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
	case Qt::Key::Key_F2:
		this->openEditor(m_currentIdx);
		break;
	case Qt::Key::Key_Delete:
		this->removeSelectedElement();
		break;
	case Qt::Key::Key_Up:
		this->moveCurrentIndex(true);
		break;
	case Qt::Key::Key_Down:
		this->moveCurrentIndex(false);
		break;
	}
}

void PathListViewWidget::applyEditorString()
{
	const int v_cur_idx = m_editor->currentIndex();

	//This means that we are appending a new element
	if (v_cur_idx == m_pathStorage.size())
	{
		m_pathStorage.push_back(m_editor->text());
		return;
	}

	if (!this->isIndexValid(v_cur_idx))
		return;

	m_pathStorage[v_cur_idx] = m_editor->text();
	this->repaint();
}
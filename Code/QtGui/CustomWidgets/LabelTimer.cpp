#include "LabelTimer.hpp"

LabelTimer::LabelTimer(const QString& text, QWidget* parent)
	: QLabel(text, parent),
	m_timer(new QTimer(this)) {}

LabelTimer::LabelTimer(QWidget* parent)
	: QLabel(parent),
	m_timer(new QTimer(this)) {}

//////////////////CENTERED LABEL TIMER//////////////////

CenteredLabelTimer::CenteredLabelTimer(
	const QString& text,
	QWidget* lbl_parent,
	QWidget* layout_parent)
	: CenteredLabel(text, lbl_parent, layout_parent),
	m_timer(new QTimer(this)) {}

CenteredLabelTimer::CenteredLabelTimer(const QString& text, QWidget* parent)
	: CenteredLabel(text, parent),
	m_timer(new QTimer(this)) {}

CenteredLabelTimer::CenteredLabelTimer(QWidget* parent)
	: CenteredLabel(parent),
	m_timer(new QTimer(this)) {}
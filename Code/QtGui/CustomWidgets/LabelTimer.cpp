#include "LabelTimer.hpp"

LabelTimer::LabelTimer(const QString& text, QWidget* parent)
	: QLabel(text, parent),
	m_timer(new QTimer(this)) {}

LabelTimer::LabelTimer(QWidget* parent)
	: QLabel(parent),
	m_timer(new QTimer(this)) {}
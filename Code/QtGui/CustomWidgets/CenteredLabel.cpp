#include "CenteredLabel.hpp"

CenteredLabel::CenteredLabel(
	const QString& text,
	QWidget* label_parent,
	QWidget* layout_parent)
	: QLabel(text, label_parent),
	m_centerLayout(new QHBoxLayout(layout_parent))
{
	this->setup();
}

CenteredLabel::CenteredLabel(const QString& text, QWidget* parent)
	: QLabel(text, parent),
	m_centerLayout(new QHBoxLayout(parent))
{
	this->setup();
}

CenteredLabel::CenteredLabel(QWidget* parent)
	: QLabel(parent),
	m_centerLayout(new QHBoxLayout(parent))
{
	this->setup();
}

void CenteredLabel::setup()
{
	m_centerLayout->addWidget(this, 0, Qt::AlignCenter);
	this->setFocusPolicy(Qt::FocusPolicy::NoFocus);
}
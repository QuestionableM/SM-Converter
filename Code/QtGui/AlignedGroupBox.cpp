#include "AlignedGroupBox.hpp"

/////////////////ALIGNED GROUP BOX////////////////

AlignedGroupBox::AlignedGroupBox(QWidget* parent)
	: QGroupBox(parent),
	m_layout(new QVBoxLayout(this))
{
	m_layout->setAlignment(Qt::AlignTop);
}

AlignedGroupBox::AlignedGroupBox(QWidget* parent, const QString& name)
	: QGroupBox(name, parent),
	m_layout(new QVBoxLayout(this))
{
	m_layout->setAlignment(Qt::AlignTop);
}

////////////OBJECT NAME BOX////////////////

ObjectNameBox::ObjectNameBox(QWidget* parent)
	: AlignedGroupBox(parent)
{
	m_lineEdit = new QLineEdit(this);
	m_layout->addWidget(m_lineEdit);
}

/////////////NAMED COMBO BOX/////////////////

NamedComboBox::NamedComboBox(QWidget* parent)
	: AlignedGroupBox(parent)
{
	m_comboBox = new QComboBox(this);
	m_layout->addWidget(m_comboBox);
}
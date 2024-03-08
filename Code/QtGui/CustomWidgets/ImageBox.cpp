#include "ImageBox.hpp"

#include <QPainter>

ImageBox::ImageBox(const QString& path, QWidget* parent)
	: QWidget(parent),
	m_image(new QImage(path))
{}

void ImageBox::paintEvent(QPaintEvent* event)
{
	QPainter v_paint(this);

	if (m_image)
		v_paint.drawImage(this->rect(), *m_image);
}
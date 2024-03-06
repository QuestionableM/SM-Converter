#pragma once

#include <QWidget>
#include <QImage>

class ImageBox : public QWidget
{
public:
	ImageBox(const QString& path, QWidget* parent = nullptr);
	~ImageBox() = default;

private:
	void paintEvent(QPaintEvent* event) override;

	QImage* m_image;
};
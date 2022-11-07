#pragma once
#include <QtWidgets>
#include <QPainter>
class PaintWdg : QWidget
{
public:
	PaintWdg(QByteArray &buff_img, QWidget* parent = nullptr);
	void showWdg(QString fileName);

protected:
	void paintEvent(QPaintEvent* event)override;

private:
	QPainter painter;
	QImage img;
};
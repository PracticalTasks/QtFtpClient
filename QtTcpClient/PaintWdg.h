#pragma once
#include <QtWidgets>
#include <QPainter>
class PaintWdg : QWidget
{
public:
	PaintWdg(QByteArray pic, QWidget* parent = nullptr);
	~PaintWdg();
	void show_wdg();

protected:
	void paintEvent(QPaintEvent* event)override;

private:
	QPainter painter;
	QImage img;
};
#pragma once
#include <QtWidgets>
#include <QPainter>
class PaintWdg : QWidget
{
public:
	PaintWdg(std::vector<char> pic, QWidget* parent = nullptr);
	void show_wdg();

protected:
	void paintEvent(QPaintEvent* event)override;

private:
	QPainter painter;
	QImage img;
	//QByteArrayView* arr;
};
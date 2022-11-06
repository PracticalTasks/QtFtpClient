#include "PaintWdg.h"

PaintWdg::PaintWdg(QByteArray &buff_img, QWidget* parent)
    : QWidget(parent)
{   
    img.loadFromData(buff_img);
}

void PaintWdg::show_wdg()
{
    show();
    repaint();
}

void PaintWdg::paintEvent(QPaintEvent* event)
{
    painter.begin(this);
    painter.drawImage(0, 0, img);
    painter.end();
}
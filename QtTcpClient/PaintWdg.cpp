#include "PaintWdg.h"

PaintWdg::PaintWdg(QByteArray pic, QWidget* parent)
    : QWidget(parent)
    //, buff_img(pic)
    
{   
    img.loadFromData(pic, "JPG");
}

PaintWdg::~PaintWdg()
{
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
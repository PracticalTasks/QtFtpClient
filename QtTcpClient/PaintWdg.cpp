#include "PaintWdg.h"

PaintWdg::PaintWdg(std::vector<char> pic, QWidget* parent)
    :
    QWidget(parent)
    //arr(new QByteArrayView(pic))
{
    img.loadFromData(pic);
}

void PaintWdg::show_wdg()
{
    show();
    repaint();
}


void PaintWdg::paintEvent(QPaintEvent* event)
{
    painter.begin(this);
    
    //img = new QImage("C:/Netwk/pic.png");

    painter.drawImage(0, 0, img);
    painter.end();
}
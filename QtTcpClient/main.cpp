#include "QtTcpClient.h"
//#include "Netwrk.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtTcpClient w;
    w.show();
    //Netwrk ntwrk(7777);
    return a.exec();
}

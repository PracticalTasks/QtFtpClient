#pragma once

#include <QtWidgets/QMainWindow>
#include <QImage>
#include <QByteArrayView>
//#include <QPainter>
#include "ui_QtTcpClient.h"
//#include "PaintWdg.h"

#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>
#include <socket_wrapper/socket_class.h>

class PaintWdg;

class QtTcpClient : public QMainWindow
{
    Q_OBJECT

public:
    QtTcpClient(QWidget *parent = nullptr);
    ~QtTcpClient();

private slots:
    void connecting_toserv();
    void req_file();
 
private:
    addrinfo* get_addrinfo(const std::string& host_name);

private:
    Ui::QtTcpClientClass ui;
    
    PaintWdg* paint_wdg;
    //QByteArrayView* arr;


    socket_wrapper::SocketWrapper sock_wrap;
    socket_wrapper::Socket* client_sock = nullptr;
    char ip[INET_ADDRSTRLEN];
    const uint32_t BUFF_SIZE = 0x3200000;       //50 Ìב
};

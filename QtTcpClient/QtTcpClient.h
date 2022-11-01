#pragma once

#include <QtWidgets/QMainWindow>
#include <QImage>
#include <QByteArrayView>
#include <QMessageBox>
#include <QHostInfo>
#include <QTcpSocket>
#include "ui_QtTcpClient.h"

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
    void slot_connecting_toserv();
    //void req_file();
    void slot_receive_file();
    //void serv_shutdown();
    void slot_errorconnect(QAbstractSocket::SocketError err);
 
private:
    uint32_t size_extraction(QByteArray &buf_bin);

private:
    Ui::QtTcpClientClass ui;
    QTcpSocket* serv_sock = nullptr;

    const uint32_t BUFF_SIZE = 4096;
    //Размер служебной информации
    const uint8_t SERV_INFO_SZ = 4;

    PaintWdg* paint_wdg = nullptr;
    socket_wrapper::SocketWrapper sock_wrap;
    //socket_wrapper::Socket* client_sock = nullptr;
    //Буфер для приёма данных частями
    QByteArray receive_buff;
    //std::vector<char>reciev_buff;
    int32_t packet_size = 0;
};

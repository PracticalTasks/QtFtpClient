#pragma once

#include <QtWidgets/QMainWindow>
#include <QImage>
#include <QByteArrayView>
#include <QMessageBox>
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
    void connecting_toserv();
    void req_file();
    void serv_shutdown();
 
private:
    addrinfo* get_addrinfo(const std::string& host_name);
    uint32_t size_extraction(std::vector<char> &buf_bin);

private:
    Ui::QtTcpClientClass ui;
    
    PaintWdg* paint_wdg = nullptr;
    socket_wrapper::SocketWrapper sock_wrap;
    socket_wrapper::Socket* client_sock = nullptr;
    uint32_t packet_size = 0;
    char ip[INET_ADDRSTRLEN]{};
    const uint32_t BUFF_SIZE = 4096;       
};

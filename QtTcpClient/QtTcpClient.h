#pragma once
#include <QtWidgets/QMainWindow>
#include <QImage>
#include <QMessageBox>
#include <QTcpSocket>
#include "ui_QtTcpClient.h"
#include "PaintWdg.h"

class QtTcpClient : public QMainWindow
{
    Q_OBJECT

public:
    QtTcpClient(QWidget *parent = nullptr);

private slots:
    void slotConnectingToServ();
    void slotReceiveFile();
    //void serv_shutdown();
    void slotErrorConnect(QAbstractSocket::SocketError err);
 
private:
    uint32_t sizeExtraction(QByteArray &buf_bin);

private:
    Ui::QtTcpClientClass ui;
    std::unique_ptr<QTcpSocket> serverSock;
    std::unique_ptr<PaintWdg> paintWdg;
    //Буфер для сбора данных вместе
    std::unique_ptr<QByteArray> fileBuff;

    const uint32_t BUFF_SIZE = 4096;
    //Размер служебной информации
    const uint8_t SERVICE_INFO_SZ = 4;

    //Буфер для приёма части данных
    QByteArray receiveBuff;

    uint32_t fileSize = 0xFFFFFFFF;  
    int32_t packetSize = 0;
    //Переменная для определения первого куска данных в котором размер файла 
    bool hasFileSize = false;
};

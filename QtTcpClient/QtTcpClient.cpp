#include "QtTcpClient.h"

QtTcpClient::QtTcpClient(QWidget *parent)
    : QMainWindow(parent)
    , receiveBuff(BUFF_SIZE, 0)
{
    ui.setupUi(this);
    
    connect(ui.pushButton_connect, SIGNAL(clicked()), SLOT(slotConnectingToServ()));
    connect(ui.pushButton_req, &QPushButton::clicked, [this]()
        {
            std::string send_mess = ui.lineEdit_addr->text().toStdString();
            serverSock->write(send_mess.data());
        });

    //connect(ui.pushButton_shutdown, &QPushButton::clicked, this, &QtTcpClient::serv_shutdown);
    //connect(ui.pushButton_disconnect, &QPushButton::clicked, [&]() {shutdown(*client_sock, 2); ui.label_connect->setText("Нет"); });
}

void QtTcpClient::slotConnectingToServ()
    {
    serverSock = std::make_unique<QTcpSocket>(this);

    connect(serverSock.get(), &QTcpSocket::connected, this, [&]() 
        { ui.output_wdg->append("Connected server with address " + 
            ui.lineEdit_ip->text() + ':' + ui.lineEdit_port->text());
          ui.label_connect->setText("ДА");
        });

    connect(serverSock.get(), &QTcpSocket::disconnected, this, [&]()
        { ui.output_wdg->append("Disconnected server with address " +
            ui.lineEdit_ip->text() + ':' + ui.lineEdit_port->text());
    ui.label_connect->setText("Нет");
        });

    connect(serverSock.get(), SIGNAL(error(QAbstractSocket::SocketError)),
        this, SLOT(slotErrorConnect(QAbstractSocket::SocketError))
    );
    connect(serverSock.get(), SIGNAL(readyRead()),
        this, SLOT(slotReceiveFile())
    );

    serverSock->connectToHost(ui.lineEdit_ip->text(), ui.lineEdit_port->text().toUInt());
}

void QtTcpClient::slotReceiveFile()
{
    //Проверяем первый ли кусок данных
    if (serverSock->bytesAvailable() >= BUFF_SIZE && (!hasFileSize))
    {
        packetSize = serverSock->read(receiveBuff.data(), BUFF_SIZE);
        if (packetSize == -1)
        {
            QMessageBox::critical(this, "Error reciev file", "Reading on a closed socket or after a process has died");
            return;
        }
        //Получаем размер файла из первых четырёх байт
        fileSize = sizeExtraction(receiveBuff);
        //Буфер для сбора картинки по чистям
        fileBuff = std::make_unique<QByteArray>(receiveBuff.data() + SERVICE_INFO_SZ, receiveBuff.size() - SERVICE_INFO_SZ);
        hasFileSize = true;
    }

    //Ждём пока не прейдут все данные
    //Размер файла - первый принятый кусок или 0xFFFFEFFF для ожидания полного заполнения буфера
    //Чтобы получить размер файла
    if (serverSock->bytesAvailable() < fileSize - BUFF_SIZE)    
        return;
    
    int32_t reciev_size = packetSize;
    while (reciev_size < fileSize)
    {

        packetSize = serverSock->read(receiveBuff.data(), BUFF_SIZE);
        if (packetSize == -1)
        {
            QMessageBox::critical(this, "Error reciev file", "Reading on a closed socket or after a process has died");
            return;
        }

        fileBuff->append(receiveBuff.data(), packetSize);
        reciev_size += packetSize;
    }
    hasFileSize = false;

    paintWdg = std::make_unique<PaintWdg>(*fileBuff.get());
    paintWdg->show_wdg();
}

//void QtTcpClient::serv_shutdown()
//{
//    const std::string CMD_EXT("exit");
//    if (client_sock->opened())
//    {
//        packetSize = send(*client_sock, CMD_EXT.c_str(), CMD_EXT.size(), 0);
//
//        if (packetSize == SOCKET_ERROR)
//        {
//            std::string err("Can't send CMD_EXT to Server. Error #" + sock_wrap.get_last_error_string());
//            QMessageBox::warning(this, "Error reciev file", err.c_str(), QMessageBox::Cancel);
//            return;
//        }
//        ui.label_status->setText("СЕРВЕР ЛЕЖИТ");
//    }
//}

void QtTcpClient::slotErrorConnect(QAbstractSocket::SocketError err)
{
    QString strError =
        "Error: " + (err == QAbstractSocket::HostNotFoundError ?
            "The host was not found." :
            err == QAbstractSocket::RemoteHostClosedError ?
            "The remote host is closed." :
            err == QAbstractSocket::ConnectionRefusedError ?
            "The connection was refused." :
            QString(serverSock->errorString())
            );
    ui.output_wdg->append(strError);
}

uint32_t QtTcpClient::sizeExtraction(QByteArray &buf_bin)
{
    uint32_t fileSize{};
    
    for (int i{}, factor{ 1 }; i < 4; ++i, factor *= 0x100)
        fileSize +=  static_cast<unsigned char>(buf_bin[i]) * factor;
        
    return fileSize;
}



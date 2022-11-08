#include "QtTcpClient.h"

QtTcpClient::QtTcpClient(QWidget *parent)
    : QMainWindow(parent)
    , receiveBuff(BUFF_SIZE, 0)
{
    ui.setupUi(this);
    ui.output_wdg->setReadOnly(true);
    connect(ui.pushButton_connect, &QPushButton::clicked, this, &QtTcpClient::slotConnectingToServ);
    connect(ui.pushButton_req, &QPushButton::clicked, [this]() {sendRequest(ui.lineEdit_addr->text()); });
    connect(ui.pushButton_shutdown, &QPushButton::clicked, this, &QtTcpClient::slotServShutdown);
}

void QtTcpClient::slotConnectingToServ()
    {
    serverSock = std::make_unique<QTcpSocket>(this);

    connect(serverSock.get(), &QTcpSocket::connected, this, [&]() 
        { 
            ui.output_wdg->append("Connected server with address " + 
            ui.lineEdit_ip->text() + ':' + ui.lineEdit_port->text());
            ui.label_connect->setText("ДА");
            //ui.label_status->setText("СОСТОЯНИЕ СЕРВЕРА");
        });

    connect(serverSock.get(), &QTcpSocket::disconnected, this, [&]()
        { 
            serverSock->waitForDisconnected();
            ui.output_wdg->append("Disconnected server with address " +
            ui.lineEdit_ip->text() + ':' + ui.lineEdit_port->text());
            ui.label_connect->setText("Нет");
        });

    connect(serverSock.get(), SIGNAL(error(QAbstractSocket::SocketError)),
        SLOT(slotErrorConnect(QAbstractSocket::SocketError)));

    connect(serverSock.get(), &QTcpSocket::readyRead, this, &QtTcpClient::slotReceiveFile);

    connect(ui.pushButton_disconnect, &QPushButton::clicked, [this]()
        {
            serverSock->disconnectFromHost();
        });

    serverSock->connectToHost(ui.lineEdit_ip->text(), ui.lineEdit_port->text().toUInt());
}

void QtTcpClient::slotSendRequest(QString request)
{
    int32_t dataSentSz = 0;
    dataSentSz = serverSock->write(request.toStdString().data());
    if (packetSize == -1)
    {
        QMessageBox::critical(this, "Error send request", "Cannot send file request");
        return;
    }
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
        //Ограничения на размер файла 4Гб
        if (fileSize > 0xFFFFFFFF)
        {
            ui.output_wdg->append("Too large file size");
            serverSock->abort();
        }
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
    paintWdg->showWdg(ui.lineEdit_addr->text());
}

void QtTcpClient::slotServShutdown()
{

    if (serverSock)
    {
        if (serverSock->state() == QAbstractSocket::ConnectedState)
        {
            sendRequest("exit");
            ui.label_status->setText("СЕРВЕР ЛЕЖИТ");
        }

    }
}

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

void QtTcpClient::sendRequest(QString request)
{
    int32_t dataSentSz = 0;
    dataSentSz = serverSock->write(request.toStdString().data());
    if (packetSize == -1)
    {
        QMessageBox::critical(this, "Error send request", "Cannot send file request");
        return;
    }
}

uint32_t QtTcpClient::sizeExtraction(QByteArray &buf_bin)
{
    uint32_t fileSize{};
    
    for (int i{}, factor{ 1 }; i < 4; ++i, factor *= 0x100)
        fileSize +=  static_cast<unsigned char>(buf_bin[i]) * factor;
        
    return fileSize;
}



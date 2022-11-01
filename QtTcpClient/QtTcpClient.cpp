#include "QtTcpClient.h"
#include "PaintWdg.h"
#pragma warning(disable : 4996)

QtTcpClient::QtTcpClient(QWidget *parent)
    : QMainWindow(parent)
    , receive_buff(BUFF_SIZE, 0)
{
    ui.setupUi(this);
    
    connect(ui.pushButton_connect, SIGNAL(clicked()), SLOT(slot_connecting_toserv()));
    connect(ui.pushButton_req, &QPushButton::clicked, [this]()
        {
            std::string send_mess = ui.lineEdit_addr->text().toStdString();
            serv_sock->write(send_mess.data());
        });
    //connect(ui.pushButton_shutdown, &QPushButton::clicked, this, &QtTcpClient::serv_shutdown);
    //connect(ui.pushButton_disconnect, &QPushButton::clicked, [&]() {shutdown(*client_sock, 2); ui.label_connect->setText("Нет"); });

}

QtTcpClient::~QtTcpClient()
{
    delete serv_sock;
    delete paint_wdg;
}

void QtTcpClient::slot_connecting_toserv()
{
    serv_sock = new QTcpSocket(this);

    connect(serv_sock, &QTcpSocket::connected, this, [&]() 
        { ui.output_wdg->append("Connected server with address " + 
            ui.lineEdit_ip->text() + ':' + ui.lineEdit_port->text());
          ui.label_connect->setText("ДА");
        });
    connect(serv_sock, SIGNAL(error(QAbstractSocket::SocketError)),
        this, SLOT(slot_errorconnect(QAbstractSocket::SocketError))
    );
    connect(serv_sock, SIGNAL(readyRead(), SLOT()),
        this, SLOT(slot_receive_file())
    );

    serv_sock->connectToHost(ui.lineEdit_ip->text(), ui.lineEdit_port->text().toUInt());
}

//void QtTcpClient::req_file()
//{
//    std::string send_mess = ui.lineEdit_addr->text().toStdString();
//
//    serv_sock->write(send_mess.data());

    ////packet_size = recv(*client_sock, reciev_buff.data(), reciev_buff.size(), 0);

    //if (packet_size == SOCKET_ERROR)
    //{
    //    std::string err("Can`t receiv file from Server. Error #" + sock_wrap.get_last_error_string());
    //    QMessageBox::warning(this, "Error reciev file", err.c_str(), QMessageBox::Cancel);

    //    return;
    //}

    ////Получаем размер файла из первых четырёх байт
    //uint32_t size_file = size_extraction(reciev_buff);
    ////Буфер для сбора картинки по чистям
    //QByteArray buff_img(reciev_buff.data() + SERV_INFO_SZ, reciev_buff.size() - SERV_INFO_SZ);

    //int32_t reciev_size{ packet_size};
    //while (reciev_size < size_file)
    //{
    //    packet_size = recv(*client_sock, reciev_buff.data(), reciev_buff.size(), 0);
    //    if (packet_size == SOCKET_ERROR)
    //    {
    //        std::string err("Can`t receiv file from Server. Error #" + sock_wrap.get_last_error_string());
    //        QMessageBox::warning(this, "Error reciev file", err.c_str(), QMessageBox::Cancel);
    //        return;
    //    }

    //    buff_img.append(reciev_buff.data(), packet_size);
    //    reciev_size += packet_size;
    //}

    //paint_wdg = new PaintWdg(buff_img);
    //paint_wdg->show_wdg();
//}

void QtTcpClient::slot_receive_file()
{
    packet_size = serv_sock->read(receive_buff.data(), BUFF_SIZE);

    if (packet_size == -1)
    {
        std::string err("Can`t receiv file from Server. Error #" + sock_wrap.get_last_error_string());
        QMessageBox::warning(this, "Error reciev file", err.c_str(), QMessageBox::Cancel);

        return;
    }

    //Получаем размер файла из первых четырёх байт
    uint32_t size_file = size_extraction(receive_buff);
    //Буфер для сбора картинки по чистям
    QByteArray buff_img(receive_buff.data() + SERV_INFO_SZ, receive_buff.size() - SERV_INFO_SZ);
    //receive_buff.fill(0);

    
    int32_t reciev_size{ packet_size };
    while (reciev_size < size_file)
    {
        packet_size = serv_sock->read(receive_buff.data(), BUFF_SIZE);
        //packet_size = recv(*client_sock, receive_buff.data(), receive_buff.size(), 0);
        if (packet_size == -1)
        {
            std::string err("Can`t receiv file from Server. Error #" + sock_wrap.get_last_error_string());
            QMessageBox::warning(this, "Error reciev file", err.c_str(), QMessageBox::Cancel);
            return;
        }

        buff_img.append(receive_buff.data(), packet_size);
        //receive_buff.fill(0);
        reciev_size += packet_size;
    }

    packet_size = 0;
    paint_wdg = new PaintWdg(buff_img);
    paint_wdg->show_wdg();
}

//void QtTcpClient::serv_shutdown()
//{
//    const std::string CMD_EXT("exit");
//    if (client_sock->opened())
//    {
//        packet_size = send(*client_sock, CMD_EXT.c_str(), CMD_EXT.size(), 0);
//
//        if (packet_size == SOCKET_ERROR)
//        {
//            std::string err("Can't send CMD_EXT to Server. Error #" + sock_wrap.get_last_error_string());
//            QMessageBox::warning(this, "Error reciev file", err.c_str(), QMessageBox::Cancel);
//            return;
//        }
//        ui.label_status->setText("СЕРВЕР ЛЕЖИТ");
//    }
//}

void QtTcpClient::slot_errorconnect(QAbstractSocket::SocketError err)
{
    QString strError =
        "Error: " + (err == QAbstractSocket::HostNotFoundError ?
            "The host was not found." :
            err == QAbstractSocket::RemoteHostClosedError ?
            "The remote host is closed." :
            err == QAbstractSocket::ConnectionRefusedError ?
            "The connection was refused." :
            QString(serv_sock->errorString())
            );
    ui.output_wdg->append(strError);
}

uint32_t QtTcpClient::size_extraction(QByteArray&buf_bin)
{
    uint32_t size_file{};
    
    for (int i{}, factor{ 1 }; i < 4; ++i, factor *= 0x100)
        size_file +=  static_cast<unsigned char>(buf_bin[i]) * factor;
        
    return size_file;
}



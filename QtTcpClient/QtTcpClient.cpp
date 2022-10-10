#include "QtTcpClient.h"
#include "PaintWdg.h"
#pragma warning(disable : 4996)

QtTcpClient::QtTcpClient(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    
    connect(ui.pushButton_connect, SIGNAL(clicked()), SLOT(connecting_toserv()));
    connect(ui.pushButton_req, SIGNAL(clicked()), SLOT(req_file()));
    connect(ui.pushButton_shutdown, &QPushButton::clicked, this, &QtTcpClient::serv_shutdown);
    connect(ui.pushButton_disconnect, &QPushButton::clicked, [&]() {shutdown(*client_sock, 2); ui.label_connect->setText("Нет"); });

}

QtTcpClient::~QtTcpClient()
{
    delete client_sock;
    //delete arr;
    delete paint_wdg;
}

void QtTcpClient::req_file()
{
    std::vector<char>buff(BUFF_SIZE);
    std::string send_mess = ui.lineEdit_addr->text().toStdString();
   // std::string send_buff;

    packet_size = send(*client_sock, send_mess.c_str(), send_mess.size(), 0);

    if (packet_size == SOCKET_ERROR)
    {
        std::cout << "Can't send message to Server. Error #";
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        return;
    }

    packet_size = recv(*client_sock, buff.data(), buff.size(), 0);

    if (packet_size == SOCKET_ERROR)
    {
        std::cout << "Can`t receiv message from Server. Error #";
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        return;
    }
    else
    {
        paint_wdg = new PaintWdg(buff);
        paint_wdg->show_wdg();
        //arr = new QByteArrayView(buff);
        //std::cout << "Server message: " << buff.data() << std::endl;
    }
        

}

void QtTcpClient::serv_shutdown()
{
    const std::string CMD_EXT("exit");
    if (client_sock->opened())
    {
        packet_size = send(*client_sock, CMD_EXT.c_str(), CMD_EXT.size(), 0);

        if (packet_size == SOCKET_ERROR)
        {
            std::cout << "Can`t receiv message from Server. Error #";
            std::cerr << sock_wrap.get_last_error_string() << std::endl;
            return;
        }

        ui.label_status->setText("СЕРВЕР ЛЕЖИТ");
    }


}

//void QtTcpClient::disconnect_fromserv()
//{
//}

//Метод для трансяции имени хоста в адресс
//Возвращает указатель на связанный список структур addrinfo содержащую информацию о хосте
addrinfo* QtTcpClient::get_addrinfo(const std::string& host_name)
{
    // Need for Windows initialization.
    //socket_wrapper::SocketWrapper sock_wrap;

    addrinfo* servinfo = nullptr;

    addrinfo hints =
    {
        .ai_flags = AI_CANONNAME,
        //ipv4
        .ai_family = AF_INET,
        // TCP stream-sockets.
        .ai_socktype = SOCK_STREAM,
        // Any protocol.
        .ai_protocol = 0
    };

    int status{};

    if ((status = getaddrinfo(host_name.c_str(), nullptr, &hints, &servinfo)) != 0)
    {
        std::cerr << "getaddrinfo error!" << std::endl;
        return nullptr;
    }

    return servinfo;
}

void QtTcpClient::connecting_toserv()
{
    client_sock = new socket_wrapper::Socket(AF_INET, SOCK_STREAM, NULL);

    if (!client_sock)
    {
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        return;
    }

    std::string str_servaddr = ui.lineEdit_ip->text().toStdString();
    if (str_servaddr[0] < '0' || str_servaddr[0] > '9')
    {
        addrinfo* servinfo = get_addrinfo(str_servaddr);
        sockaddr_in const* const sin = reinterpret_cast<const sockaddr_in* const>(servinfo->ai_addr);
        inet_ntop(AF_INET, &(sin->sin_addr), ip, INET_ADDRSTRLEN);
        str_servaddr = ip;
        freeaddrinfo(servinfo);
    }

    sockaddr_in serv_addr = {
    .sin_family = AF_INET,
    .sin_port = htons(ui.lineEdit_port->text().toUInt())
    };

    serv_addr.sin_addr.s_addr = inet_addr(str_servaddr.c_str());

    if (::connect(*client_sock, (SOCKADDR*)&serv_addr, sizeof(serv_addr)))
    {
        std::cout << "Connection to Server is FAILED. Error #";
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        return;
    }

    ui.label_connect->setText("ДА");
}



#include "QtTcpClient.h"
#include "PaintWdg.h"
#pragma warning(disable : 4996)

QtTcpClient::QtTcpClient(QWidget *parent)
    : QMainWindow(parent)
    , reciev_buff(BUFF_SIZE)
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
    delete paint_wdg;
}

void QtTcpClient::connecting_toserv()
{
    client_sock = new socket_wrapper::Socket(AF_INET, SOCK_STREAM, NULL);

    if (!client_sock)
    {
        QMessageBox::warning(this, "Socket creation error", sock_wrap.get_last_error_string().c_str(), QMessageBox::Cancel);
        return;
    }

    std::string str_servaddr = ui.lineEdit_ip->text().toStdString();
    char ip[INET_ADDRSTRLEN]{};
    //Сделать более глубокий анализ строки!!!
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
        std::string err("Connection to Server is FAILED. Error #" + sock_wrap.get_last_error_string());
        QMessageBox::warning(this, "Error connection to server", err.c_str(), QMessageBox::Cancel);
        return;
    }

    ui.label_connect->setText("ДА");
}

void QtTcpClient::req_file()
{
    std::string send_mess = ui.lineEdit_addr->text().toStdString();

    packet_size = send(*client_sock, send_mess.c_str(), send_mess.size(), 0);

    if (packet_size == SOCKET_ERROR)
    {
        std::string err("Can't send request of file to Server. Error #" + sock_wrap.get_last_error_string());
        QMessageBox::warning(this, "Error sending request of file", err.c_str(), QMessageBox::Cancel);
        return;
    }

    packet_size = recv(*client_sock, reciev_buff.data(), reciev_buff.size(), 0);

    if (packet_size == SOCKET_ERROR)
    {
        std::string err("Can`t receiv file from Server. Error #" + sock_wrap.get_last_error_string());
        QMessageBox::warning(this, "Error reciev file", err.c_str(), QMessageBox::Cancel);

        return;
    }

    //Получаем размер файла из первых четырёх байт
    uint32_t size_file = size_extraction(reciev_buff);
    //Буфер для сбора картинки по чистям
    QByteArray buff_img(reciev_buff.data() + SZ_SERV_INFO, reciev_buff.size() - SZ_SERV_INFO);

    int32_t reciev_size{ packet_size};
    while (reciev_size < size_file)
    {
        packet_size = recv(*client_sock, reciev_buff.data(), reciev_buff.size(), 0);
        if (packet_size == SOCKET_ERROR)
        {
            std::string err("Can`t receiv file from Server. Error #" + sock_wrap.get_last_error_string());
            QMessageBox::warning(this, "Error reciev file", err.c_str(), QMessageBox::Cancel);
            return;
        }

        buff_img.append(reciev_buff.data(), packet_size);
        reciev_size += packet_size;
    }

    paint_wdg = new PaintWdg(buff_img);
    paint_wdg->show_wdg();
}

void QtTcpClient::serv_shutdown()
{
    const std::string CMD_EXT("exit");
    if (client_sock->opened())
    {
        packet_size = send(*client_sock, CMD_EXT.c_str(), CMD_EXT.size(), 0);

        if (packet_size == SOCKET_ERROR)
        {
            std::string err("Can't send CMD_EXT to Server. Error #" + sock_wrap.get_last_error_string());
            QMessageBox::warning(this, "Error reciev file", err.c_str(), QMessageBox::Cancel);
            return;
        }
        ui.label_status->setText("СЕРВЕР ЛЕЖИТ");
    }
}

//Метод для трансяции имени хоста в адресс
//Возвращает указатель на связанный список структур addrinfo содержащую информацию о хосте
addrinfo* QtTcpClient::get_addrinfo(const std::string& host_name)
{
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

    if (status = getaddrinfo(host_name.c_str(), nullptr, &hints, &servinfo))
    {
        QMessageBox::warning(this, "Error getting getaddrinfo", "getaddrinfo error!", QMessageBox::Cancel);
        return nullptr;
    }

    return servinfo;
}

uint32_t QtTcpClient::size_extraction(std::vector<char> &buf_bin)
{
    uint32_t size_file{};
    
    for (int i{}, factor{ 1 }; i < 4; ++i, factor *= 0x100)
        size_file +=  static_cast<unsigned char>(buf_bin[i]) * factor;
        
    return size_file;
}



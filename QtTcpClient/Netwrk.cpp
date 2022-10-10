#include "Netwrk.h"

Netwrk::Netwrk(const uint16_t WRITE_PORT)
{
    write(WRITE_PORT);
}


int Netwrk::write(const uint16_t WRITE_PORT)
{
    std::cout << "Starting echo client ...\n";

    socket_wrapper::SocketWrapper sock_wrap;
    socket_wrapper::Socket client_sock = { AF_INET, SOCK_STREAM, NULL };
    const char IP_SERV[] = "192.168.50.254";

    if (!client_sock)
    {
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        return EXIT_FAILURE;
    }

    sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(WRITE_PORT)
    };

    serv_addr.sin_addr.s_addr = inet_addr(IP_SERV);

    if (::connect(client_sock, (SOCKADDR*)&serv_addr, sizeof(serv_addr)))
    {
        std::cout << "Connection to Server is FAILED. Error #";
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        return EXIT_FAILURE;
    }

    //
    std::cout << "Connected! ...\n";

    //std::string buffer;
    //char recv_buffer[256]{};
    //int recv_len{};
    //socklen_t client_addrlen = sizeof(sockaddr_in);

    std::vector<char>buff(BUFF_SIZE);
    std::string send_mess;
    std::string send_buff;

    short packet_size = 0;

    while (true)
    {
        std::cout << "Your (Client) message to Server: ";
        fgets(buff.data(), buff.size(), stdin);

        send_mess = buff.data();

        packet_size = send(client_sock, send_mess.c_str(), send_mess.size() - 1, 0);

        if (packet_size == SOCKET_ERROR)
        {
            std::cout << "Can't send message to Server. Error #";
            std::cerr << sock_wrap.get_last_error_string() << std::endl;
            return EXIT_FAILURE;
        }

        if (str_tolower(send_mess) == CMD_EXT)
        {
            shutdown(client_sock, SD_BOTH);
            std::cout << "Stopping echo server ...\n";
            std::cout << "Disconected\n";
            return 0;
        }

        packet_size = recv(client_sock, buff.data(), buff.size(), 0);

        if (packet_size == SOCKET_ERROR)
        {
            std::cout << "Can`t receiv message from Server. Error #";
            std::cerr << sock_wrap.get_last_error_string() << std::endl;
            return EXIT_FAILURE;
        }
        else
            std::cout << "Server message: " << buff.data() << std::endl;
    }

    return EXIT_SUCCESS;
}

std::string Netwrk::str_tolower(const std::string& str)
{
    std::string str_lower;
    for (int i{}; i < str.size() - 1; ++i)
        str_lower += tolower(str[i]);
    return str_lower;
}
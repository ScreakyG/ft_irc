#include "Colors.hpp"
#include "Server.hpp"
#include "Random.hpp"

#include <stdio.h>

void sendClientHeader(const int &clientFd)
{
    int sentBytes = -1;
    std::string msg = "---------------------------\n---WELCOME TO THE SERVER---\n---------------------------\n";

    sentBytes = send(clientFd, msg.c_str(), msg.size(), 0);
    if (sentBytes == -1)
        std::cout << RED << "[Server] sending error : " << std::strerror(errno) << RESET << std::endl;
    else if (sentBytes != static_cast<int>(msg.size()))
        std::cout << YELLOW << "[Server] Partial message was sent to client." << RESET << std::endl;

}

int main(void)
{
    Server server;

    try
    {
        server.createServerSocket();
        server.listenPort();

        // Definier si on utilise select(), poll(), epoll() ou kqueue();
        int client_fd = accept(server.getServerSocket(), NULL, NULL);
        sendClientHeader(client_fd);
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}

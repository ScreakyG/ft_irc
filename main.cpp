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
        std::cerr << RED << "[Server] sending error : " << std::strerror(errno) << RESET << std::endl;
    else if (sentBytes != static_cast<int>(msg.size()))
        std::cout << YELLOW << "[Server] Partial message was sent to client." << RESET << std::endl;
}

int main(void)
{
    Server server;

    try
    {
        server.createServerSocket();
        server.createIpv4Address(SERVER_IP, SERVER_PORT);
        server.bindServerSocket();
        server.listenPort();

        // Definier si on utilise select(), poll(), epoll() ou kqueue();
        int         client_fd;
        sockaddr_in client;
        socklen_t   clientSize = sizeof(client);

        client_fd = accept(server.getServerSocket(), reinterpret_cast<sockaddr *>(&client), &clientSize);
        if (client_fd == -1)
            std::cerr << "Couldn't connect client." << std::endl;

        sendClientHeader(client_fd);

        char    buffer[BUFSIZ];
        while (true)
        {
            // Clear the buffer.
            memset(buffer, '\0', BUFSIZ);
            int  amountReceived = recv(client_fd, buffer, BUFSIZ, 0);
            if (amountReceived <= 0)
            {
                if (amountReceived == -1)
                    std::cerr << RED << "[Server] recv error : " << std::strerror(errno) << RESET << std::endl;
                else if (amountReceived == 0)
                    std::cout << "[" << client_fd << "]" << " : Closed connection." << std::endl;
                break ;
            }
            else
            {
                buffer[amountReceived] = '\0';
                std::cout << "[" << client_fd << "] : " << buffer;
            }
        }
        close(client_fd);
        //close(server.getServerSocket());
        shutdown(server.getServerSocket(), SHUT_RDWR);

    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return (0);

}

// lsof -i :PORT   Cette commande permet de voir les connexions etablies.

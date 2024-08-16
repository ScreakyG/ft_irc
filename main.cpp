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

// void    testFunction(Server &server)
// {
//     int         client_fd;
//     sockaddr_in client;
//     socklen_t   clientSize = sizeof(client);

//     client_fd = accept(server.getServerSocket(), reinterpret_cast<sockaddr *>(&client), &clientSize);
//     if (client_fd == -1)
//         std::cerr << "Couldn't connect client." << std::endl;

//     sendClientHeader(client_fd);

//     char    buffer[BUFSIZ];
//     while (true)
//     {
//         // Clear the buffer.
//         memset(buffer, '\0', BUFSIZ);
//         int  amountReceived = recv(client_fd, buffer, BUFSIZ, 0);
//         if (amountReceived <= 0)
//         {
//             if (amountReceived == -1)
//                 std::cerr << RED << "[Server] recv error : " << std::strerror(errno) << RESET << std::endl;
//             else if (amountReceived == 0)
//                 std::cout << "[" << client_fd << "]" << " : Closed connection." << std::endl;
//             break ;
//         }
//         else
//         {
//             buffer[amountReceived] = '\0';
//             std::cout << "[" << client_fd << "] : " << buffer;
//         }
//     }
//         close(client_fd);
//         //close(server.getServerSocket());
//         shutdown(server.getServerSocket(), SHUT_RDWR);
// }


void    accept_new_client(Server &server)
{
    int     newClientFd = accept(server.getServerSocket(), NULL, NULL);
    pollfd  newClient;

    if (newClientFd == -1)
        std::cerr << "[Server] Couldn't connect new client." << std::endl;
    else
    {
        newClient.fd = newClientFd;
        newClient.events = POLLIN;
        server.allSockets.push_back(newClient);

        sendClientHeader(newClientFd);
    }
}

void read_data_from_socket(int i, Server &server)
{
    int     amountReceived;
    char    buffer[BUFSIZ];

    amountReceived = recv(server.allSockets[i].fd, buffer, BUFSIZ, 0);
    if (amountReceived <= 0)
    {
         if (amountReceived == -1)
            std::cerr << RED << "[Server] recv error : " << std::strerror(errno) << RESET << std::endl;
        else if (amountReceived == 0)
            std::cout << "[" << server.allSockets[i].fd << "]" << " : Closed connection." << std::endl;
        close(server.allSockets[i].fd);
        server.allSockets.erase(server.allSockets.begin() + i); // Il faut surement delete le client qui a deconnecte.
    }
    else
    {
        buffer[amountReceived] = '\0';
        std::cout << "[" << server.allSockets[i].fd << "] : " << buffer;
    }
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

        //testFunction(server); // Just a test function.

        pollfd  serverFd;

        serverFd.fd = server.getServerSocket();
        serverFd.events = POLLIN;

        server.allSockets.push_back(serverFd);
        std::cout << PURPLE << "[Sever] Set up poll fd array." << RESET << std::endl;

        while (true)
        {
            int status = poll(&server.allSockets[0], server.allSockets.size(), 2000);
            if (status == -1)
            {
                std::cerr << RED << "[Server] Poll error : " << std::strerror(errno) << RESET << std::endl;
                break ;
            }
            else if (status == 0)
            {
                std::cout << PURPLE << "[Server] Waiting..." << RESET << std::endl;
                continue ;
            }

            for (int i = 0; i < static_cast<int>(server.allSockets.size()); i++)
            {
                if ((server.allSockets[i].revents & POLLIN) != 1)
                    continue ;
                std::cout << "[" << server.allSockets[i].fd << "] Ready for I/O operation" << std::endl;

                if (server.allSockets[i].fd == server.getServerSocket())
                    accept_new_client(server);
                else
                    read_data_from_socket(i, server);
            }
        }


    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return (0);

}

// lsof -i :PORT   Cette commande permet de voir les connexions etablies.

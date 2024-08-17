#include "Colors.hpp"
#include "Server.hpp"
#include "Random.hpp"
#include "Client.hpp"



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
    int port = DEFAULT_PORT;
    Server server(port);

    try
    {
        signal(SIGINT, Server::signalHandler);

        server.serverInit();
        server.startServerRoutine();
        server.closeAllFds();
    }
    catch (const std::exception &e)
    {
        server.closeAllFds();
        std::cerr << e.what() << std::endl;
    }
    return (0);

}

// lsof -i :PORT   Cette commande permet de voir les connexions etablies.

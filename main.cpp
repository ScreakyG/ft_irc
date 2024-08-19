#include "includes/Colors.hpp"
#include "includes/Server.hpp"
#include "includes/Random.hpp"
#include "includes/Client.hpp"
#include "includes/Commands.hpp"


void printInfoBanner(const Server &server)
{
    (void)server;
    std::cout << "-----------------------------------" << std::endl;
    std::cout << "-------- IRC SERVER CONFIG :--------" << std::endl;
    std::cout << "-----------------------------------" << std::endl;
    std::cout << "--IP : " << SERVER_IP << " ---------" << std::endl;
    std::cout << "--PORT : " << DEFAULT_PORT <<" ------" << std::endl;
    std::cout << "--PASSWORD : " << DEFAULT_PASSWORD << " -----------------------------" << std::endl;
    std::cout << "-----------------------------------" << std::endl;
}

int main(void)
{
    int         port = DEFAULT_PORT;
    std::string password = DEFAULT_PASSWORD;
    Server server(port, password);

    printInfoBanner(server);
    try
    {
        signal(SIGINT, Server::signalHandler);
        signal(SIGQUIT, Server::signalHandler);

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
// Peut etre rajouter l' option non bloquante sur les sockets ?

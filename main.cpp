#include "includes/Colors.hpp"
#include "includes/Server.hpp"
#include "includes/Random.hpp"
#include "includes/Client.hpp"
#include "includes/Commands.hpp"
#include "includes/Channel.hpp"


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

long checkPort(const char* portStr)
{
    const int IRC_PORT_MIN = 6665;
    const int IRC_PORT_MAX = 6669;
    const int IRC_SSL_PORT = 6697;

    if (!portStr || portStr[0] == '\0')
        throw std::invalid_argument("Error: Empty port");

    for (int i = 0; portStr[i]; i++) 
    {
        if (portStr[i] < '0' || portStr[i] > '9')
            throw std::invalid_argument("Error: Port must contain only digits");
    }
    char* endPtr;
    long port = std::strtol(portStr, &endPtr, 10);
    
    if (*endPtr != '\0')
        throw std::invalid_argument("Error: Invalid port number");
    if (port < 1024)
        throw std::invalid_argument("Error: Ports below 1024 are reserved");
    if (port > 65535)
        throw std::invalid_argument("Error: Port must be between 1024 and 65535");

    if (DEBUG == FULL)
    {
        if ((port < IRC_PORT_MIN || port > IRC_PORT_MAX) && port != IRC_SSL_PORT)
            std::cout << "Warning: Non-standard IRC port (standard: 6665-6669, 6697)" << std::endl;
    }
    return port;
}

std::string checkPassword(const char* passStr)
{
    if (!passStr || passStr[0] == '\0')
        throw std::invalid_argument("Error: Empty password");
    return std::string(passStr);
}


int main(int ac, char **av)
{
    if (ac != 3) 
    {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    try 
    {
        long port = checkPort(av[1]);
        std::string password = checkPassword(av[2]);
        Server server(port, password);
        printInfoBanner(server);
        
        try 
        {
            signal(SIGINT, Server::signalHandler);
            signal(SIGQUIT, Server::signalHandler);
            
            server.serverInit();
            server.startServerRoutine();
            server.deleteAllClients();
            server.deleteAllChannels();
            server.closeAllFds();
        }
        catch (const std::exception &e) 
        {
            server.deleteAllClients();
            server.deleteAllChannels();
            server.closeAllFds();
            std::cerr << e.what() << std::endl;
        }
    }
    catch (const std::exception &e) 
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

// lsof -i :PORT   Cette commande permet de voir les connexions etablies.
// Peut etre rajouter l' option non bloquante sur les sockets ?

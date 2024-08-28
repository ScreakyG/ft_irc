#include "../includes/Commands.hpp"

void exec_CAP(Server &server, std::vector<std::string> &arguments, int clientFd)
{
    Client      *client;
    std::string message;

    client = server.getClientStruct(clientFd);
    if (client == NULL)
    {
        std::cout << RED << "[" << clientFd << "] [Server] Client is not connected to server" << RESET << std::endl;
        return ;
    }

    if (arguments.size() > 0)
    {
        if (arguments[0] == "END")
            return ;
        else
        {
            message = ":localhost CAP * LS :\r\n";
            server.sendToClient(message, clientFd);
            return ;
        }
    }
}

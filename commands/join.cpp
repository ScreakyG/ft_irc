#include "../includes/Commands.hpp"

void exec_JOIN(Server &server, std::vector<std::string> &arguments, int clientFd)
{
    Client      *client;
    std::string message;


    client = server.getClientStruct(clientFd);
    if (client == NULL)
    {
        std::cout << RED << "[" << clientFd << "] [Server] Client is not connected to server" << RESET << std::endl;
        return ;
    }

    if (arguments.size() == 0)
    {
        message = ERR_NEEDMOREPARAMS(client->getNickname(), "JOIN");
        server.sendToClient(message, clientFd);
        return ;
    }
    else
    {
        message = "Your COMMAND will be treated.\r\n";
        server.sendToClient(message, clientFd);
    }
}

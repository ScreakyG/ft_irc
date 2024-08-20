#include "../includes/Commands.hpp"

void exec_PING(Server &server, std::vector<std::string> &arguments, int clientFd)
{
    Client *    client;
    std::string message;

    client = server.getClientStruct(clientFd);
    if (client == NULL)
    {
        std::cout << RED << "[" << clientFd << "] [Server] Client is not connected to server" << RESET << std::endl;
        return ;
    }

    if (client->hasRegistered() == false)
    {
        message = ERR_NOTREGISTERED;
        server.sendToClient(message, clientFd);
        return ;
    }

    if (arguments.size() == 0)
    {
        message = ERR_NEEDMOREPARAMS(client->getNickname(), "PING"); // ERR_NOORIGIN est egalement valable.
        server.sendToClient(message, clientFd);
        return ;
    }

    if (arguments.size() == 1)
    {
        message = ":localhost PONG localhost :" + arguments[0] + "\r\n";
        server.sendToClient(message, clientFd);
        return ;
    }

    if (arguments.size() > 1)
    {
        if (arguments[1] == "localhost")
        {
            message = ":localhost PONG localhost :" + arguments[0] + "\r\n";
            server.sendToClient(message, clientFd);
            return ;
        }
        else if (arguments[1] != "localhost")
        {
            message = ERR_NOSUCHSERVER(client->getNickname(), arguments[1]);
            server.sendToClient(message, clientFd);
            return ;
        }
    }
}

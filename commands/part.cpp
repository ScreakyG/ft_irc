#include "../includes/Commands.hpp"

void exec_PART(Server &server, std::vector<std::string> &arguments, int clientFd)
{
    Client      *client;
    Channel     *channel;
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

    if (arguments.size() < 1)
    {
        message = ERR_NEEDMOREPARAMS(client->getNickname(), "PART");
        server.sendToClient(message, clientFd);
        return ;
    }

    channel = server.getChannel(arguments[0]);
    if (!channel)
    {
        message = ERR_NOSUCHCHANNEL(client->getNickname(), arguments[0]);
        server.sendToClient(message, client->getFd());
        return ;
    }

    if (channel->isUserOnChannel(client) == false)
    {
        message = ERR_NOTONCHANNEL(client->getNickname(), channel->getChannelName());
        server.sendToClient(message, client->getFd());
        return ;
    }

    message = ":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getHostname() + " PART " + channel->getChannelName() + "\r\n";
    server.sendToClient(message, client->getFd());
    client->leaveChannel(channel);
}

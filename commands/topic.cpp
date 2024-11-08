#include "../includes/Commands.hpp"

void exec_TOPIC(Server &server, std::vector<std::string> &arguments, int clientFd)
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

    if (arguments.size() == 0)
    {
        message = ERR_NEEDMOREPARAMS(client->getNickname(), "TOPIC");
        server.sendToClient(message, clientFd);
        return ;
    }

    channel = server.getChannel(arguments[0]);
    if (channel == NULL)
    {
        message = ERR_NOSUCHCHANNEL(client->getNickname(), arguments[0]);
        server.sendToClient(message, client->getFd());
        return ;
    }

    if (channel->isUserOnChannel(client) == false)
    {
        message = ERR_NOTONCHANNEL(client->getNickname(), channel->getChannelName());
        server.sendToClient(message, client->getFd());
    }

    if (arguments.size() < 2)
    {
        if (channel->getChannelTopic().empty() == true)
        {
            message = RPL_NOTOPIC(client->getNickname(), channel->getChannelName());
        }
        else
            message = RPL_TOPIC(client->getNickname(), channel->getChannelName(), channel->getChannelTopic());
        server.sendToClient(message, client->getFd());
        return ;
    }

    if (channel->isTopicRestricted() == true && channel->isUserOperator(client) == false)
    {
        message = ERR_CHANOPRIVSNEEDED(client->getNickname());
        server.sendToClient(message, client->getFd());
        return ;
    }

    channel->setTopic(arguments[1]);
    message = ":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getHostname() + " TOPIC " + channel->getChannelName() + " " + arguments[1] + "\r\n";
    channel->announceNewUser(server, message);
}

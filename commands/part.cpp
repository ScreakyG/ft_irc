#include "../includes/Commands.hpp"

std::vector<std::string> multipleArgParser(std::string string)
{
    std::vector<std::string>  splittedArgs;
    std::string               argValue;
    size_t                    pos;

    if (string.empty())
        return (splittedArgs);

    for (pos = string.find(','); pos != std::string::npos; pos = string.find(','))
    {
        argValue = string.substr(0, pos);
        splittedArgs.push_back(argValue);
        string.erase(0, pos + 1);
    }

    if(string.empty() == false)
    {
        argValue = string;
        splittedArgs.push_back(argValue);
    }

    return (splittedArgs);
}

void exec_PART(Server &server, std::vector<std::string> &arguments, int clientFd)
{
    Client                              *client;
    Channel                             *channel;
    std::vector<std::string>            splittedArgs;
    std::vector<std::string>::iterator  it;
    std::string                         message;

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

    splittedArgs = multipleArgParser(arguments[0]);
    for (it = splittedArgs.begin(); it != splittedArgs.end(); it++)
    {
        channel = server.getChannel(*it);
        if (!channel)
        {
            message = ERR_NOSUCHCHANNEL(client->getNickname(), *it);
            server.sendToClient(message, client->getFd());
            continue ;
        }
        if (channel->isUserOnChannel(client) == false)
        {
            message = ERR_NOTONCHANNEL(client->getNickname(), *it);
            server.sendToClient(message, client->getFd());
            continue ;
        }

        message = ":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getHostname() + " PART " + channel->getChannelName() + "\r\n";
        channel->notifyUsers(server, message, client);
        server.sendToClient(message, client->getFd());
        client->leaveChannel(channel);
    }
}

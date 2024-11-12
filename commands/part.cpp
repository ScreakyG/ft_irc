#include "../includes/Commands.hpp"

static std::vector<std::string> getChannelsToLeave(std::string channelsString)
{
    std::vector<std::string>  channels;
    std::string               channelName;
    size_t                    pos;

    if (channelsString.empty())
        return (channels);

    for (pos = channelsString.find(','); pos != std::string::npos; pos = channelsString.find(','))
    {
        channelName = channelsString.substr(0, pos);
        channels.push_back(channelName);
        channelsString.erase(0, pos + 1);
    }

    if(channelsString.empty() == false)
    {
        channelName = channelsString;
        channels.push_back(channelName);
    }
    
    return (channels);
}

void exec_PART(Server &server, std::vector<std::string> &arguments, int clientFd)
{
    Client                              *client;
    Channel                             *channel;
    std::vector<std::string>            channels;
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

    channels = getChannelsToLeave(arguments[0]);
    for (it = channels.begin(); it != channels.end(); it++)
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
        server.sendToClient(message, client->getFd());
        client->leaveChannel(channel);
    }


    // channel = server.getChannel(arguments[0]);
    // if (!channel)
    // {
    //     message = ERR_NOSUCHCHANNEL(client->getNickname(), arguments[0]);
    //     server.sendToClient(message, client->getFd());
    //     return ;
    // }

    // if (channel->isUserOnChannel(client) == false)
    // {
    //     message = ERR_NOTONCHANNEL(client->getNickname(), channel->getChannelName());
    //     server.sendToClient(message, client->getFd());
    //     return ;
    // }

    // message = ":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getHostname() + " PART " + channel->getChannelName() + "\r\n";
    // server.sendToClient(message, client->getFd());
    // client->leaveChannel(channel);
}

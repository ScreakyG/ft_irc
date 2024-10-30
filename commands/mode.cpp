#include "../includes/Commands.hpp"

static void setFlag(Server &server, Client *client, Channel *channel, char flag)
{
    if (flag == 'i')
        channel->setInviteMode(true);
    (void)server;
    (void)client;
}

static void readFlags(Server &server, std::vector<std::string> &arguments, Client *client, Channel *channel)
{
    std::string flags;
    std::string message;


    if (channel->isUserOperator(client) == false)
    {
        message = ERR_CHANOPRIVSNEEDED(client->getNickname());
        server.sendToClient(message, client->getFd());
        return ;
    }

    flags = arguments[1];
    for (size_t idx = 0; idx < flags.size(); idx++)
    {
        setFlag(server, client, channel, flags[idx]);
    }

}

void exec_MODE(Server &server, std::string &ogString ,std::vector<std::string> &arguments, int clientFd)
{
    Client      *client;
    Channel     *channel;
    std::string message;
    std::string channelName;

    client = server.getClientStruct(clientFd);
    if (client == NULL)
    {
        std::cout << RED << "[" << clientFd << "] [Server] Client is not connected to server" << RESET << std::endl;
        return ;
    }

    if (arguments.size() == 0)
    {
        message = ERR_NEEDMOREPARAMS(client->getNickname(), "MODE");
        server.sendToClient(message, clientFd);
        return ;
    }

    channelName = arguments[0];

    if (server.channelExist(channelName) == false)
    {
        message = ERR_NOSUCHCHANNEL(client->getNickname(), channelName);
        server.sendToClient(message, client->getFd());
        return ;
    }

    channel = server.getChannel(channelName);
    if (!channel)
        return ;

    readFlags(server, arguments, client, channel);
    (void)ogString;
}

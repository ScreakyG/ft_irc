#include "../includes/Commands.hpp"

static bool isFlagSupported(char flag)
{
    std::string supportedFlags = "itkol";

    for (size_t idx = 0; idx < supportedFlags.size(); idx++)
    {
        if (flag == supportedFlags[idx])
            return (true);
    }
    return (false);
}

static void setFlag(Server &server, Client *client, Channel *channel, char flag, bool removeMode)
{
    std::string message;

    if (channel->isUserOperator(client) == false)
    {
        message = ERR_CHANOPRIVSNEEDED(client->getNickname());
        server.sendToClient(message, client->getFd());
        return ;
    }

    //////////////////////////////////////////

    if (flag == 'i')
        modifyInviteMode(server, client, channel, removeMode);
}

static void readFlags(Server &server, std::vector<std::string> &arguments, Client *client, Channel *channel)
{
    std::string flags;
    std::string message;
    bool        removeMode = false;

    flags = arguments[1];

    // Si un seul des flags est non valide alors on ne fait rien.
    for (size_t idx = 0; idx < flags.size(); idx++)
    {
        if (idx == 0 && (flags[0] == '-' || flags[0] == '+'))
            continue ;
        if (isFlagSupported(flags[idx]) == false)
        {
            std::string flagString;

            flagString += flags[idx];
            message = ERR_UNKNOWNMODE(client->getNickname(), flagString);
            server.sendToClient(message, client->getFd());
            return ;
        }
    }

    // Les flags sont valides alors on les executes.
    for (size_t idx = 0; idx < flags.size(); idx++)
    {
        if (idx == 0 && (flags[0] == '-' || flags[0] == '+'))
        {
            if (flags[0] == '-')
                removeMode = true;
            continue ;
        }
        setFlag(server, client, channel, flags[idx], removeMode);
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

    if (arguments.size() < 2)
        return ; // Changer car cela veut dire que la commande est du style MODE #CARS

    readFlags(server, arguments, client, channel);
    (void)ogString;
}

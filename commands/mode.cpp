#include "../includes/Commands.hpp"

static bool isValidFlagsString(std::string &flagsString)
{
    std::string::iterator   it;

    if (flagsString.empty() == true)
        return (false);

    for (it = flagsString.begin(); it != flagsString.end(); it ++)
    {
        if (*it != '-' || *it != '+')
            return (true);
    }
    return (false);
}

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

static bool validFlagArguments(std::string &flagsString, int idx, unsigned long &arg_idx, std::vector<std::string> &arguments, bool removeMode)
{
    if (flagsString[idx] == 'i' || flagsString[idx] == 't' || (removeMode == true && (flagsString[idx] == 'k' || flagsString[idx] == 'l'))) //Ces flags n'aurons jamais besoin d'arguments.
        return (true);

    // On regarde les flags qui necessiste un argument.
    if (arguments.size() < 3 + arg_idx)
        return (false);

    if (arguments[2 + arg_idx].empty() == true)
        return (false);

    std::cout << "ARGUMENT FOR FLAG : " << flagsString[idx] << " = " << arguments[2 + arg_idx] << "\n";
    arg_idx++;

    return (true);
}

static void setFlag(Server &server, Client *client, Channel *channel, std::vector<std::string> &arguments, char flag, unsigned long &arg_idx, bool removeMode, std::string &successfullModes, std::string &successfullFlagsArgs)
{
    std::string message;

    if (flag == 'i')
       successfullModes += modifyInviteMode(server, client, channel, removeMode);
    else if (flag == 't')
        successfullModes += modifiyTopicRestrictions(server, client, channel, removeMode);

    else if (flag == 'o')
    {
        successfullModes += modifyOperators(server, client, channel, removeMode, arguments[2 + arg_idx], successfullFlagsArgs);
        arg_idx++;
    }
    else if (flag == 'k')
    {
        if (removeMode == true)
            successfullModes += modifyChannelKey(server, client, channel, removeMode, "", successfullFlagsArgs);
        else
        {
            successfullModes += modifyChannelKey(server, client, channel, removeMode, arguments[2 + arg_idx], successfullFlagsArgs);
            arg_idx++;
        }
    }
    else if (flag == 'l')
    {
        if (removeMode == true)
            successfullModes += modifyChannelUsersLimit(server, client, channel, removeMode, "", successfullFlagsArgs);
        else
        {
            successfullModes += modifyChannelUsersLimit(server, client, channel, removeMode, arguments[2 + arg_idx], successfullFlagsArgs);
            arg_idx++;
        }
    }
}

static void readFlags(Server &server, std::vector<std::string> &arguments, Client *client, Channel *channel)
{
    std::string flags;
    std::string message;
    bool        removeMode = false;

    std::string successfullModes;
    std::string succesfullFlagsArgs;
    unsigned long arg_idx = 0;

    flags = arguments[1]; // Debut des flags.

    // Faire une verification pour regarder si la chaine est pas vide ou si elle ne contient pas que des '-' ou '+'.
    if (isValidFlagsString(flags) == false)
        return ;

    // Si un seul des flags est non valide alors on ne fait rien.
    for (size_t idx = 0; idx < flags.size(); idx++)
    {
        if (flags[idx] == '-' || flags[idx] == '+')
        {
            if (flags[idx] == '-')
                removeMode = true;
            else
                removeMode = false;
            continue ;
        }
        if (isFlagSupported(flags[idx]) == false)
        {
            std::string flagString;

            flagString += flags[idx];
            message = ERR_UNKNOWNMODE(client->getNickname(), flagString);
            server.sendToClient(message, client->getFd());
            return ;
        }
        if (validFlagArguments(flags, idx, arg_idx, arguments, removeMode) == false)
        {
            message = ERR_NEEDMOREPARAMS(client->getNickname(), "MODE");
            server.sendToClient(message, client->getFd());
            return ;
        }
    }

    // Les flags sont valides alors on les executes.

    removeMode = 0;
    arg_idx = 0;
    for (size_t idx = 0; idx < flags.size(); idx++)
    {
        if (flags[idx] == '-' || flags[idx] == '+')
        {
            if (flags[idx] == '-')
                removeMode = true;
            else
                removeMode = false;
            continue ;
        }
        setFlag(server, client, channel, arguments, flags[idx], arg_idx, removeMode, successfullModes, succesfullFlagsArgs);
    }

    if (successfullModes.empty() == false)
        sendModeReply(server, client, channel, successfullModes, succesfullFlagsArgs);
}

void exec_MODE(Server &server, std::string &ogString ,std::vector<std::string> &arguments, int clientFd)
{
    (void)ogString;

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

    if (client->hasRegistered() == false)
    {
        message = ERR_NOTREGISTERED;
        server.sendToClient(message, clientFd);
        return ;
    }

    if (arguments.size() == 0)
    {
        message = ERR_NEEDMOREPARAMS(client->getNickname(), "MODE");
        server.sendToClient(message, clientFd);
        return ;
    }

    channelName = arguments[0];
    channel = server.getChannel(channelName);
    if (channel == NULL)
    {
        message = ERR_NOSUCHCHANNEL(client->getNickname(), channelName);
        server.sendToClient(message, client->getFd());
        return ;
    }

    if (channel->isUserOnChannel(client) == false)
    {
        message = ERR_NOTONCHANNEL(client->getNickname(), channelName)
        server.sendToClient(message, client->getFd());
        return ;
    }

    // Cela veut dire que l'input est du style MODE #CARS.
    if (arguments.size() < 2)
    {
        message = RPL_CHANNELMODEIS(client->getNickname(), channelName, channel->getChannelModes());
        server.sendToClient(message, client->getFd());
        return ;
    }

    if (channel->isUserOperator(client) == false)
    {
        message = ERR_CHANOPRIVSNEEDED(client->getNickname());
        server.sendToClient(message, client->getFd());
        return ;
    }

    readFlags(server, arguments, client, channel);
}

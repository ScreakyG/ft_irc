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

static bool validFlagArguments(std::string &flagsString, int idx, unsigned long &arg_idx, std::vector<std::string> &arguments)
{
    if (flagsString[idx] == 'i' || flagsString[idx] == 't') //Ces flags n'aurons jamais besoin d'arguments.
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

    if (channel->isUserOperator(client) == false)
    {
        message = ERR_CHANOPRIVSNEEDED(client->getNickname());
        server.sendToClient(message, client->getFd());
        return ;
    }

    //////////////////////////////////////////

    if (flag == 'i')
       successfullModes += modifyInviteMode(server, client, channel, removeMode);
    else if (flag == 't')
        successfullModes += modifiyTopicRestrictions(server, client, channel, removeMode);

    else if (flag == 'o')
    {
        successfullModes += modifyOperators(server, client, channel, removeMode, arguments[2 + arg_idx], successfullFlagsArgs);
        arg_idx++;
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
            continue ;
        if (isFlagSupported(flags[idx]) == false)
        {
            std::string flagString;

            flagString += flags[idx];
            message = ERR_UNKNOWNMODE(client->getNickname(), flagString);
            server.sendToClient(message, client->getFd());
            return ;
        }
        if (validFlagArguments(flags, idx, arg_idx, arguments) == false)
        {
            message = ERR_NEEDMOREPARAMS(client->getNickname(), "MODE");
            server.sendToClient(message, client->getFd());
            return ;
        }
    }

    // Les flags sont valides alors on les executes.

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
}

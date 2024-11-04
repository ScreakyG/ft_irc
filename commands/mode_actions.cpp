#include "../includes/Commands.hpp"
#include <limits>

void sendModeReply(Server &server, Client *client, Channel *channel, std::string flags, std::string flagsArguments)
{
    std::string message;

    message = ":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getHostname() + " MODE " + channel->getChannelName();
    message += " " + flags;

    if (flagsArguments.empty() == false)
        message += " " + flagsArguments;
    message += "\r\n";

    // Broadcast to every user on channel.
    channel->announceNewUser(server, message);
    return ;
}

std::string modifyInviteMode(Server &server, Client *client, Channel *channel, bool removeMode)
{
    (void)server;
    (void)client;

    if (removeMode == false && channel->isInviteOnly() == false)
    {
        channel->setInviteMode(true);
        return ("+i");
    }
    else if (removeMode == true && channel->isInviteOnly() == true)
    {
        channel->setInviteMode(false);
        return ("-i");
    }
    return ("");
}

std::string modifiyTopicRestrictions(Server &server, Client *client, Channel *channel, bool removeMode)
{
    (void)server;
    (void)client;

    if (removeMode == false && channel->isTopicRestricted() == false)
    {
        channel->setTopicRestrictions(true);
        return ("+t");
    }
    else if (removeMode == true && channel->isTopicRestricted() == true)
    {
        channel->setTopicRestrictions(false);
        return ("-t");
    }
    return ("");
}

std::string modifyOperators(Server &server, Client *client, Channel *channel, bool removeMode, std::string argumentName, std::string &successfullFlagsArgs)
{
    Client* clientToModify;
    std::string message;

    clientToModify = channel->getClientOnChannel(argumentName);

    // Le client n'est pas dans le channel.
    if (!clientToModify)
    {
        message = ERR_NOSUCHNICK(client->getNickname(), argumentName);
        server.sendToClient(message, client->getFd());
        return ("");
    }

    if (removeMode == false)
    {
        if (channel->isUserOperator(clientToModify) == false)
            channel->addOperator(clientToModify);
        successfullFlagsArgs += argumentName + " ";
        return ("+o");
    }
    else if (removeMode == true)
    {
        if (channel->isUserOperator(clientToModify) == true)
            channel->quitOperator(clientToModify);
        successfullFlagsArgs += argumentName + " ";
        return ("-o");
    }
    return ("");
}

std::string modifyChannelKey(Server &server, Client *client, Channel *channel, bool removeMode, std::string argumentName, std::string &successfullFlagsArgs)
{
    (void)server;
    (void)client;

    if (removeMode == true && channel->getChannelPassword().empty() == false)
    {
        channel->changePassword("");
        successfullFlagsArgs += "* ";
        return ("-k");
    }
    else if (removeMode == false)
    {
        channel->changePassword(argumentName);
        successfullFlagsArgs += argumentName + " ";
        return ("+k");
    }
    return ("");
}

std::string modifyChannelUsersLimit(Server &server, Client *client, Channel *channel, bool removeMode, std::string argumentName, std::string &successfullFlagsArgs)
{
    (void)server;
    (void)client;

    unsigned long   newLimit;
    char   *endptr;

    errno = 0;
    if (removeMode == true && channel->getUsersLimit() != 0)
    {
        channel->changeUsersLimit(0);
        return ("-l");
    }
    else if (removeMode == false)
    {
        if (argumentName.find("-") != std::string::npos)
            return ("");
        newLimit = std::strtoul(argumentName.c_str(), &endptr, 10);
        if (*endptr != '\0' || errno == ERANGE)
            return ("");
        channel->changeUsersLimit(newLimit);
        successfullFlagsArgs += argumentName + " ";
        return ("+l");
    }
    return ("");
}

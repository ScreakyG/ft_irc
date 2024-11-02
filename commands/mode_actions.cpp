#include "../includes/Commands.hpp"

void sendModeReply(Server &server, Client *client, Channel *channel, std::string flags)
{
    std::string message;

    message = ":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getHostname() + " MODE " + channel->getChannelName();
    message += " " + flags + "\r\n";

    //server.sendToClient(message, client->getFd());

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
        //sendModeReply(server, client, channel, "+i");

        return ("+i");
    }
    else if (removeMode == true && channel->isInviteOnly() == true)
    {
        channel->setInviteMode(false);
        //sendModeReply(server, client, channel, "-i");

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

// std::string modifyOperators(Server &server, Client *client, Channel *channel, bool removeMode)
// {

// }

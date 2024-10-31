#include "../includes/Commands.hpp"

void sendModeReply(Server &server, Client *client, Channel *channel, std::string flags)
{
    std::string message;

    message = ":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getHostname() + " MODE " + channel->getChannelName();
    message += " " + flags + "\r\n";
    server.sendToClient(message, client->getFd());

    return ;
}

void modifyInviteMode(Server &server, Client *client, Channel *channel, bool removeMode)
{
    if (removeMode == false && channel->isInviteOnly() == false)
    {
        channel->setInviteMode(true);
        sendModeReply(server, client, channel, "+i");
    }
    else if (removeMode == true && channel->isInviteOnly() == true)
    {
        channel->setInviteMode(false);
        sendModeReply(server, client, channel, "-i");
    }
}

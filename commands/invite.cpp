#include "../includes/Commands.hpp"

void exec_INVITE(Server &server, std::vector<std::string> &arguments, int clientFd)
{
    Client      *client;
    Client      *clientToInvite;
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

    if (arguments.size() < 2)
    {
        message = ERR_NEEDMOREPARAMS(client->getNickname(), "INVITE");
        server.sendToClient(message, clientFd);
        return ;
    }

    clientToInvite = server.getClientByName(arguments[0]);
    if (clientToInvite == NULL)
    {
        message = ERR_NOSUCHNICK(client->getNickname(), arguments[0]);
        server.sendToClient(message, client->getFd());
        return ;
    }

    channel = server.getChannel(arguments[1]);
    if (channel == NULL)
    {
        message = ERR_NOSUCHCHANNEL(client->getNickname(), arguments[1]);
        server.sendToClient(message, client->getFd());
        return ;
    }

    if (channel->isUserOnChannel(client) == false)
    {
        message = ERR_NOTONCHANNEL(client->getNickname(), channel->getChannelName());
        server.sendToClient(message, client->getFd());
        return ;
    }

    if (channel->isInviteOnly() == true && channel->isUserOperator(client) == false)
    {
        message = ERR_CHANOPRIVSNEEDED(client->getNickname());
        server.sendToClient(message, client->getFd());
        return ;
    }

    if (channel->isUserOnChannel(clientToInvite) == true)
    {
        message = ERR_USERONCHANNEL(client->getNickname(), clientToInvite->getNickname(), channel->getChannelName());
        server.sendToClient(message, client->getFd());
        return ;
    }

    // Message pour celui qui invite.
    message = RPL_INVITING(client->getNickname(), channel->getChannelName(), clientToInvite->getNickname());
    server.sendToClient(message, client->getFd());

    // Message pour celui qui recoit l'invite.
    message = ":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getHostname() + " INVITE " + clientToInvite->getNickname() + " :" + channel->getChannelName() + "\r\n";
    server.sendToClient(message, clientToInvite->getFd());

    // Ajout de l'utilisateur dans la whitelist du channel.

    channel->getInvitedUsersVector().push_back(clientToInvite);
}

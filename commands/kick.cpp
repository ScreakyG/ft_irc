#include "../includes/Commands.hpp"

static void kickUser(Server &server, Channel *channel, Client *client, Client *target)
{
    std::string message;

    // Verifier que le client n'est pas egalement la target.
    // Que se passe t'il si on se kick soit meme et que on etait le dernier user du channel ? Si il reste quelqun, devient t'il operateur ?


    message = ":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getHostname() + " KICK " + channel->getChannelName() + " " + target->getNickname() + " :" + target->getNickname() + "\r\n";
    channel->announceNewUser(server, message);
    target->leaveChannel(channel);
    channel->quitClient(target);
}

void exec_KICK(Server &server, std::string &ogString, std::vector<std::string> &arguments, int clientFd)
{
    (void)ogString;

    Client      *client;
    Client      *target;
    Channel     *channel;
    std::string message;

    client = server.getClientStruct(clientFd);
    if (client == NULL)
    {
        std::cout << RED << "[" << clientFd << "] [Server] Client is not connected to server" << RESET << std::endl;
        return ;
    }

    if (arguments.size() < 2)
    {
        message = ERR_NEEDMOREPARAMS(client->getNickname(), "KICK");
        server.sendToClient(message, clientFd);
        return ;
    }

    channel = server.getChannel(arguments[0]);
    if (channel == NULL)
    {
        message = ERR_NOSUCHCHANNEL(client->getNickname(), arguments[0]);
        server.sendToClient(message, client->getFd());
        return ;
    }

    if (channel->isUserOnChannel(client) == false)
    {
        message = ERR_NOTONCHANNEL(client->getNickname(), arguments[0])
        server.sendToClient(message, client->getFd());
        return ;
    }

    if (channel->isUserOperator(client) == false)
    {
        message = ERR_CHANOPRIVSNEEDED(client->getNickname());
        server.sendToClient(message, client->getFd());
        return ;
    }

    target = channel->getClientOnChannel(arguments[1]);
    if (target == NULL)
    {
        message == ERR_NOSUCHNICK(client->getNickname(), arguments[1]);
        server.sendToClient(message, client->getFd());
        return ;
    }

    kickUser(server, channel, client, target);
}
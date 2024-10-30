#include "../includes/Commands.hpp"

void exec_PRIVMSG(Server &server, std::vector<std::string> &arguments, int clientFd)
{
    Client *sender = server.getClientStruct(clientFd);
    if (!sender)
        return;

    if (arguments.size() < 2)
    {
        std::string response = ERR_NEEDMOREPARAMS(sender->getNickname(), "PRIVMSG");
        server.sendToClient(response, clientFd);
        return;
    }

    std::string target = arguments[0];
    std::string message = arguments[1];
    
    if (message[0] == ':')
        message = message.substr(1);

    if (target[0] == '#')
    {
        Channel *channel = server.getChannel(target);
        if (!channel)
        {
            std::string response = ERR_NOSUCHCHANNEL(sender->getNickname(), target);
            server.sendToClient(response, clientFd);
            return;
        }

        if (!sender->alreadyJoined(target))
        {
            std::string response = ERR_NOTONCHANNEL(sender->getNickname(), target);
            server.sendToClient(response, clientFd);
            return;
        }

        std::string reply = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHostname() + " PRIVMSG " + target + " :" + message + "\r\n";
        channel->announceNewUser(server, reply);
    }
    else
    {
        Client *recipient = NULL;
        for (size_t i = 0; i < FD_SETSIZE; i++)
        {
            Client *client = server.getClientStruct(i);
            if (client && client->getNickname() == target)
            {
                recipient = client;
                break;
            }
        }

        if (!recipient)
        {
            std::string response = ERR_NOSUCHNICK(sender->getNickname(), target);
            server.sendToClient(response, clientFd);
            return;
        }

        std::string reply = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHostname() + " PRIVMSG " + target + " :" + message + "\r\n";
        server.sendToClient(reply, recipient->getFd());
    }
}

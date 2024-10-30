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

    if (target[0] == '#')
    {
        Channel *channel = server.getChannel(target);
        if (!channel)
        {
            std::string response = ERR_NOSUCHCHANNEL(sender->getNickname(), target);
            server.sendToClient(response, clientFd);
            return;
        }

        std::string reply = ":" + sender->getNickname() + "!" + sender->getUsername() 
                         + "@" + sender->getHostname() + " PRIVMSG " + target 
                         + " :" + message + "\r\n";

        std::vector<Client *> &channelClients = channel->getActiveUsersVector();
        std::vector<Client *>::iterator it;
        for (it = channelClients.begin(); it != channelClients.end(); ++it)
        {
            if ((*it)->getFd() != clientFd)
                server.sendToClient(reply, (*it)->getFd());
        }
    }
    else
    {
        Client *recipient = NULL;
        std::vector<Client *> &allClients = server.getVectorClient();
        std::vector<Client *>::iterator it;
        for (it = allClients.begin(); it != allClients.end(); ++it)
        {
            if ((*it)->getNickname() == target)
            {
                recipient = *it;
                break;
            }
        }

        if (!recipient)
        {
            std::string response = ERR_NOSUCHNICK(sender->getNickname(), target);
            server.sendToClient(response, clientFd);
            return;
        }

        std::string reply = ":" + sender->getNickname() + "!" + sender->getUsername() 
                         + "@" + sender->getHostname() + " PRIVMSG " + target 
                         + " :" + message + "\r\n";
        server.sendToClient(reply, recipient->getFd());
    }
}

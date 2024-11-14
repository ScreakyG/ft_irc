#include "../includes/Commands.hpp"


static std::string constructFullMessage(Client *sender, std::string message, std::string targetName)
{
    std::string fullMessage = ":" + sender->getNickname() + "!" + sender->getUsername()
                           + "@" + sender->getHostname() + " PRIVMSG " + targetName
                           + " " + message + "\r\n";

    if (fullMessage.length() > 512)
    {
        size_t prefixLen = 1 // ":"
                        + sender->getNickname().length()
                        + 1 // "!"
                        + sender->getUsername().length()
                        + 1 // "@"
                        + sender->getHostname().length()
                        + 9 // " PRIVMSG "
                        + targetName.length()
                        + 2 // " :"
                        + 2; // "\r\n"

        message = message.substr(0, 512 - prefixLen);
        fullMessage = ":" + sender->getNickname() + "!" + sender->getUsername()
                   + "@" + sender->getHostname() + " PRIVMSG " + targetName
                   + " " + message + "\r\n";
    }
    return (fullMessage);
}

void exec_PRIVMSG(Server &server, std::vector<std::string> &arguments, int clientFd)
{
    Client *sender = server.getClientStruct(clientFd);
    if (!sender)
        return;
    if (arguments.empty())
    {
        std::string response = ERR_NORECIPIENT(sender->getNickname(), "PRIVMSG");
        server.sendToClient(response, clientFd);
        return;
    }
    if (arguments.size() < 2)
    {
        std::string response = ERR_NEEDMOREPARAMS(sender->getNickname(), "PRIVMSG");
        server.sendToClient(response, clientFd);
        return;
    }

    // Construction du message initial
    std::string target = arguments[0];
    std::string message;
    for (size_t i = 1; i < arguments.size(); ++i)
    {
        message += arguments[i];
        if (i < arguments.size() - 1)
            message += " ";
    }

    // Vérification message vide
    if (message.empty() || message == "::" || message == ":")
    {
        std::string response = ERR_NOTEXTTOSEND(sender->getNickname());
        server.sendToClient(response, clientFd);
        return;
    }

    std::vector<std::string>    targets;
    Channel                     *channelTarget;
    Client                      *clientTarget;
    std::string                 fullMessage;

    // Get all targets separated by ','.
    targets = multipleArgParser(target);
    if (targets.size() > MAX_TARGETS)
    {
        std::string response = ERR_TOOMANYTARGETS(sender->getNickname());
        server.sendToClient(response, clientFd);
        return;
    }

    // Loop all targets.
    for (size_t idx = 0; idx < targets.size(); idx++)
    {
        fullMessage = constructFullMessage(sender, message, targets[idx]);
        //If target is a channel.
        if (targets[idx][0] == '#')
        {
            channelTarget = server.getChannel(targets[idx]);
            if (!channelTarget)
            {
                std::string response = ERR_NOSUCHCHANNEL(sender->getNickname(), targets[idx]);
                server.sendToClient(response, clientFd);
                continue ;
            }
            if (!channelTarget->isUserOnChannel(sender))
            {
                std::string response = ERR_CANNOTSENDTOCHAN(sender->getNickname(), targets[idx]);
                server.sendToClient(response, clientFd);
                continue ;
            }
            // fullMessage = constructFullMessage(sender, message, targets[idx]);
            channelTarget->notifyUsers(server, fullMessage, sender);
            continue ;
        }
        // Special targets.
        else if (targets[idx].find('.') != std::string::npos)
        {
            if (targets[idx].find('*') != std::string::npos || targets[idx].find('?') != std::string::npos)
            {
                std::string response = ERR_WILDTOPLEVEL(sender->getNickname(), targets[idx]);
                server.sendToClient(response, clientFd);
                continue ;
            }
            if (targets[idx].find('.') == targets[idx].length() - 1)
            {
                std::string response = ERR_NOTOPLEVEL(sender->getNickname(), targets[0]);
                server.sendToClient(response, clientFd);
                continue ;
            }
        }
        // Target are clients.
        else
        {
            clientTarget = server.getClientByName(targets[idx]);
            if (!clientTarget)
            {
                std::string response = ERR_NOSUCHNICK(sender->getNickname(), targets[idx]);
                server.sendToClient(response, clientFd);
                continue ;
            }
            fullMessage = constructFullMessage(sender, message, targets[idx]);
            server.sendToClient(fullMessage, clientTarget->getFd());
        }
    }
}

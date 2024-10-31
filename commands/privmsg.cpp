#include "../includes/Commands.hpp"

struct ClientCompare {
    ClientCompare(Client* target) : _target(target) {}
    bool operator()(const Client* client) const {
        return client == _target;
    }
    Client* _target;
};

//to do
//add CANNOTSENDTOCHAN
//fix NOTEXTTOSEND comportement
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
    std::string target = arguments[0];
    std::string message;
    for (size_t i = 1; i < arguments.size(); ++i) 
    {
        message += arguments[i];
        if (i < arguments.size() - 1)
            message += " ";
    }
    if (message.empty() || message == ":")
    {
        std::string response = ERR_NOTEXTTOSEND(sender->getNickname());
        server.sendToClient(response, clientFd);
        return;
    }
    std::vector<std::string> targets;
    size_t pos = 0;
    while ((pos = target.find(',')) != std::string::npos) 
    {
        targets.push_back(target.substr(0, pos));
        target.erase(0, pos + 1);
    }
    targets.push_back(target);
    if (targets.size() > 1)
    {
        std::string response = ERR_TOOMANYTARGETS(sender->getNickname());
        server.sendToClient(response, clientFd);
        return;
    }
    if (targets[0][0] == '#')
    {
        Channel *channel = server.getChannel(targets[0]);
        if (!channel)
        {
            std::string response = ERR_NOSUCHCHANNEL(sender->getNickname(), targets[0]);
            server.sendToClient(response, clientFd);
            return;
        }
        std::vector<Client *> &channelClients = channel->getActiveUsersVector();
        if (std::find_if(channelClients.begin(), channelClients.end(), 
            ClientCompare(sender)) == channelClients.end())
        {
            std::string response = ERR_NOTONCHANNEL(sender->getNickname(), targets[0]);
            server.sendToClient(response, clientFd);
            return;
        }
        std::string reply = ":" + sender->getNickname() + "!" + sender->getUsername() //fix gestion message
                         + "@" + sender->getHostname() + " PRIVMSG " + targets[0] 
                         + " :" + message + "\r\n";
        std::vector<Client *>::iterator it;
        for (it = channelClients.begin(); it != channelClients.end(); ++it)
        {
            if ((*it)->getFd() != clientFd)
                server.sendToClient(reply, (*it)->getFd());
        }
    }
    else
    {
        if (targets[0].find('.') != std::string::npos) 
        {
            if (targets[0].find('*') != std::string::npos || targets[0].find('?') != std::string::npos) 
            {
                std::string response = ERR_WILDTOPLEVEL(sender->getNickname(), targets[0]);
                server.sendToClient(response, clientFd);
                return;
            }
            if (targets[0].find('.') == targets[0].length() - 1) 
            {
                std::string response = ERR_NOTOPLEVEL(sender->getNickname(), targets[0]);
                server.sendToClient(response, clientFd);
                return;
            }
        }
        Client *recipient = NULL;
        std::vector<Client *> &allClients = server.getVectorClient();
        std::vector<Client *>::iterator it;
        for (it = allClients.begin(); it != allClients.end(); ++it)
        {
            if ((*it)->getNickname() == targets[0])
            {
                recipient = *it;
                break;
            }
        }
        if (!recipient)
        {
            std::string response = ERR_NOSUCHNICK(sender->getNickname(), targets[0]);
            server.sendToClient(response, clientFd);
            return;
        }
        std::string reply = ":" + sender->getNickname() + "!" + sender->getUsername() 
                         + "@" + sender->getHostname() + " PRIVMSG " + targets[0] 
                         + " :" + message + "\r\n";
        server.sendToClient(reply, recipient->getFd());
    }
}

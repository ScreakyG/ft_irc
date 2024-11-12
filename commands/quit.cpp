#include "../includes/Commands.hpp"

static bool alreadyAdded(Client *clientToAdd, std::vector<Client *> &clientsToNotify)
{
    std::vector<Client *>::iterator    it;

    for (it = clientsToNotify.begin(); it != clientsToNotify.end(); it++)
    {
        if ((*it)->getFd() == clientToAdd->getFd())
            return (true);
    }
    return (false);
}

static std::vector<Client *> getClientsToNotify(Client *sender)
{
    std::vector<Client *>               cliensToNotify;
    std::vector<Channel *>              clientChannels = sender->getClientChannel();
    std::vector<Channel *>::iterator    it;

    cliensToNotify.push_back(sender);
    for (it = clientChannels.begin(); it != clientChannels.end(); it++)
    {
        std::vector<Client *>   currentChannelUsers = (*it)->getActiveUsersVector();
        for (std::vector<Client *>::iterator it_channel = currentChannelUsers.begin(); it_channel != currentChannelUsers.end(); it_channel++)
        {
            if (alreadyAdded(*it_channel, cliensToNotify) == false)
                cliensToNotify.push_back(*it_channel);
        }
    }
    return (cliensToNotify);
}

void exec_QUIT(Server &server, std::vector<std::string> &arguments, int clientFd)
{
    Client                              *client;
    std::string                         quitMessage;
    std::string                         message;

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

    // Cera utilise dans le cas d'un ctrl+c.

    quitMessage = ":" + client->getNickname() + "!~" + client->getUsername() + "@" + client->getHostname() + " QUIT ";

    if (arguments.size() == 0)
        quitMessage += ":Remote host closed the connection.";

    for (size_t idx = 0; idx < arguments.size(); idx++)
    {
        quitMessage += arguments[idx];
        if (idx < arguments.size() - 1)
            quitMessage += " ";
    }

    quitMessage += "\r\n";

    std::vector<Client *>              clientsToNotify;
    std::vector<Client *>::iterator    it;

    clientsToNotify = getClientsToNotify(client);
    for (it = clientsToNotify.begin(); it != clientsToNotify.end(); it++)
        server.sendToClient(quitMessage, (*it)->getFd());

    // server.sendToClient(quitMessage, client->getFd());

    // std::vector<Channel *>&             clientChannels = client->getClientChannel();
    // std::vector<Channel *>::iterator    it;

    // for (it = clientChannels.begin(); it != clientChannels.end(); it++)
    //     (*it)->notifyUsers(server, quitMessage, client);

    throw Server::ClientDisconnect();
}

#include "../includes/Commands.hpp"


void   exec_PASS(Server &server, std::vector<std::string> &arguments, int clientFd)
{
    std::string message;
    Client      *client;

    client = server.getClientStruct(clientFd);
    if (client == NULL)
    {
        std::cout << RED << "[" << clientFd << "] [Server] Client is not connected to server" << RESET << std::endl;
        return ;
    }

    if (client->hasRegistered() == true)
    {
        message = ERR_ALREADYREGISTRED(client->getNickname());
        server.sendToClient(message, clientFd);
        return ;
    }

    if (arguments.size() == 0)
    {
        message = ERR_NEEDMOREPARAMS(client->getNickname(), "PASS");
        server.sendToClient(message, clientFd);
        return ;
    }

    if (server.getServerPassword() != arguments[0])
    {
        message = ERR_PASSWDMISMATCH(client->getNickname());
        server.sendToClient(message, clientFd);
        message = ERROR_MSG(std::string("Closing Link: localhost (Bad Password)"));
        server.sendToClient(message, clientFd);
        server.deleteClient(clientFd);
        return ;
    }

    if (server.getServerPassword() == arguments[0])
        client->setServerPassword(true);

    if (client->hasRegistered() == false)
        server.isRegistrationComplete(client);
}

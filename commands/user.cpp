#include "../includes/Commands.hpp"

void exec_USER(Server &server, std::vector<std::string> &arguments, int clientFd)
{
    Client      *client;
    std::string message;
    std::string username;

    client = server.getClientStruct(clientFd);
    if (client == NULL)
    {
        std::cout << RED << "[" << clientFd << "] [Server] Client is not connected to server" << RESET << std::endl;
        return ;
    }

    // Ceci etait une ancienne protection pour empecher la commande tant que PASS n'etais pas indique.
    // if (server.clientValidPassword(client, clientFd) == false)
    //     return ;

    if (client->hasRegistered() == true)
    {
        message = ERR_ALREADYREGISTRED(client->getNickname());
        server.sendToClient(message, clientFd);
        return ;
    }

    if (arguments.size() != 4)
    {
        message = ERR_NEEDMOREPARAMS(client->getNickname(), "USER");
        server.sendToClient(message, clientFd);
        return ;
    }
    else if (arguments.size() == 4 && client->hasRegistered() == false)
    {
        username = arguments[0] + " " + arguments[1] + " " + arguments[2] + " " + arguments[3];
        client->setUsername(username);
        server.isRegistrationComplete(client); //Si oui on envoie le message de bienvenue et client->setRegistered(true), si non alors on attends avec un timeout.
    }
}

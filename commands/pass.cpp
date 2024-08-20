#include "../includes/Commands.hpp"


void   exec_PASS(Server &server, std::vector<std::string> &arguments, int clientFd)
{
    std::string message;
    Client      *client;

    client = server.getClientStruct(clientFd);
    if (client == NULL)
        return ;

    if (client->hasRegistered() == true)
    {
        //message = "462 : You may not reregister\n";
        message = ERR_ALREADYREGISTRED(client->getNickname());
        server.sendToClient(message, clientFd);
        return ;
    }

    if (arguments.size() == 0)
    {
        //message = "461 PASS : Not enough parameters\n";
        message = ERR_NEEDMOREPARAMS(client->getNickname(), "PASS");
        server.sendToClient(message, clientFd);
        return ;
    }

    if (server.getServerPassword() != arguments[0])
    {
        message = "461 : Password incorrect\n"; // Ce n'est pas le bon code mais il n'y pas l'air d'avoir de bon code.
        server.sendToClient(message, clientFd);
        //deleteClient(clientFd);
        return ;
    }

    if (server.getServerPassword() == arguments[0])
    {
        client->setServerPassword(true);
        return ;
    }
}

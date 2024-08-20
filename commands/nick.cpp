#include "../includes/Commands.hpp"

static bool isValidNickname(std::string nickname)
{
    const std::string validCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    if (nickname.empty())
        return (false);
    if (nickname.size() > NICK_MAXLEN)
        return (false);
    if (nickname.find_first_not_of(validCharacters) != std::string::npos)
        return (false);
    return (true);
}

void exec_NICK(Server &server, std::vector<std::string> &arguments, int clientFd)
{
    std::string message;
    std::string nickname;
    Client      *client;

    std::vector<Client> &allClients = server.getVectorClient();

    client = server.getClientStruct(clientFd);
    if (client == NULL)
    {
        std::cout << RED << "[" << clientFd << "] [Server] Client is not connected to server" << RESET << std::endl;
        return ;
    }

    // Ceci etait une ancienne protection pour empecher la commande tant que PASS n'etais pas indique.
    // if (server.clientValidPassword(client, clientFd) == false)
    //     return ;

    if (arguments.size() == 0)
    {
        //message = std::string("431 * ") + ":No nickname given\n";
        message = ERR_NONICKNAMEGIVEN(client->getNickname());
        server.sendToClient(message, clientFd);
        return ;
    }

    for (size_t i = 0; i < arguments.size(); i++) // Concatene si par exemple "Billy Butcher" , correspond a 2 args dans le parsing , pourtant ceci peut etre valide.
        nickname += arguments[i];

    if (isValidNickname(nickname) == false) // Sur les vrais serveurs cela truncate le pseudo a MAXLEN. Faire la grosse verification ici pour les char invalides ect..
    {
        //message = std::string("432 ") + nickname + " :Erroneus Nickname\n";
        message = ERR_ERRONEUSNICKNAME(client->getNickname(), nickname);
        server.sendToClient(message, clientFd);
        return ;
    }

    for (size_t i = 0; i < allClients.size(); i++) // Regarde si n'y a pas deja quelqu'un avec ce pseudo sur le server.
    {
        if (nickname == allClients[i].getNickname())
        {
            //message = std::string("432 ") + nickname + " :Nickname is already in use\n"; // Le vrai code d'erreur est 433 , cest un choix perso d'utiliser 432 pour deconnecter le client.
            message = ERR_NICKNAMEINUSE(client->getNickname(), nickname);
            server.sendToClient(message, clientFd);
            return ;
        }
    }
    client->setNickname(nickname);
    message = "You're now known as " + client->getNickname() + "\n";
    server.sendToClient(message, clientFd);

    if (client->hasRegistered() == false)
        server.isRegistrationComplete(client);
}
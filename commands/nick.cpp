#include "../includes/Commands.hpp"

void exec_NICK(Server &server, std::vector<std::string> &arguments, int clientFd)
{
    std::string message;
    std::string nickname;
    Client      *client;

    std::vector<Client> &allClients = server.getVectorClient();

    client = server.getClientStruct(clientFd);
    if (client == NULL)
        return ;

    if (server.clientValidPassword(client, clientFd) == false)
        return ;

    if (arguments.size() == 0)
    {
        message = std::string("431 * ") + ":No nickname given\n";
        server.sendToClient(message, clientFd);
        return ;
    }

    for (size_t i = 0; i < arguments.size(); i++) // Concatene si par exemple "Billy Butcher" , correspond a 2 args dans le parsing , pourtant ceci peut etre valide.
        nickname += arguments[i];

    if (nickname.size() > NICK_MAXLEN) // Sur les vrais serveurs cela truncate le pseudo a MAXLEN. Faire la grosse verification ici pour les char invalides ect..
    {
        message = std::string("432 ") + nickname + " :Erroneus Nickname, MAX_LEN = 10\n";
        server.sendToClient(message, clientFd);
        return ;
    }

    for (size_t i = 0; i < allClients.size(); i++) // Regarde si n'y a pas deja quelqu'un avec ce pseudo sur le server.
    {
        if (nickname == allClients[i].getNickname())
        {
            //message = nickname + " :Nickname is already in use\n";
            message = std::string("432 ") + nickname + " :Nickname is already in use\n"; // Le vrai code d'erreur est 433 , cest un choix perso d'utiliser 432 pour deconnecter le client.
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

#include "../includes/Commands.hpp"

static void    joinChannels(Server &server, std::vector<std::pair<std::string, std::string> > &namesAndPasswords, Client *client)
{
    std::string message;

    if (client == NULL)
    {
        std::cout << RED << "[" << "?" << "] [Server] Client is not connected to server" << RESET << std::endl;
        return ;
    }

    for (size_t idx = 0; idx < namesAndPasswords.size(); idx++)
    {
        std::string channelName = namesAndPasswords[idx].first;
        std::string channelPassword = namesAndPasswords[idx].second;
        if (server.channelExist(channelName) == true)
        {
            //Rejoindre le channel.
            Channel *channelToJoin = server.getChannel(channelName);

            if (channelToJoin != NULL)
            {
               client->joinChannel(server, *channelToJoin, channelPassword);
            }
            else
                std::cout << "Unexpected error when joingning a Channel" << std::endl;
        }
        else
        {
            //Creer le channel.
            Channel newChannel(channelName, channelPassword);
            Channel*channelToJoin = NULL;

            server.addChannel(newChannel);
            channelToJoin = server.getChannel(channelName);
            //channelToJoin->addOperator(client); // Ajoute le createur du channel en tant que operateur.
            client->joinChannel(server, *channelToJoin, channelPassword);
        }
    }
}

static bool validChannelName(std::string &name)
{
    std::string::iterator   it;

    if (name.empty() == true)
        return (false);

    it = name.begin();
    if (*it != '#') // Le premier caractere doit etre un '#' obligatoirement
        return (false);
    it++;
    if (it == name.end())
        return (false);
    return (true);
}

void checkChannelsNamesValid(Server &server, Client *client, std::vector<std::pair<std::string, std::string> > &namesAndPasswords)
{
    std::string                                                 name;
    std::string                                                 message;
    std::vector<std::pair<std::string, std::string> >::iterator it;

    // for (it = namesAndPasswords.begin(); it != namesAndPasswords.end(); it++)
    // {
    //     std::cout << "Name = " << it->first << " | Pass = " << it->second << std::endl;
    // }

    for (it = namesAndPasswords.begin(); it != namesAndPasswords.end(); it++)
    {
        name = it->first;
        if (validChannelName(name) == false)
        {
            message = ERR_NOSUCHCHANNEL(client->getNickname(), name);
            server.sendToClient(message, client->getFd());
            namesAndPasswords.erase(it);// Peut etre remove le channel du vecteur ?
            if (namesAndPasswords.empty() == true) // Arrive dans le cas ou il y a un seul channel dans le vecteur et qu'il est faux. Segfault sinon avec begin().
                break ;
            it = namesAndPasswords.begin(); // Obliger de restart apres erase sinon y'a un segfault.
        }
    }
}

void           exec_JOIN(Server &server, std::string &ogString, std::vector<std::string> &arguments, int clientFd)
{
    Client      *client;
    std::string message;

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

    if (arguments.size() == 0)
    {
        message = ERR_NEEDMOREPARAMS(client->getNickname(), "JOIN");
        server.sendToClient(message, clientFd);
        return ;
    }

    ////////////////////////////////////

    trimString(ogString, " ");
    ogString.erase(ogString.find("JOIN"), 4);

    std::vector<std::pair<std::string, std::string> > vector;

    vector = parseInput(server, client, ogString);

    if (vector.size() == 0) // Protege dans le cas ou aucun channel a un nom valide. Donc un vecteur vide.
    {
        std::cout << RED << "Error channel parsing, no valid channel was entered" << RESET << std::endl;
        return ;
    }
   joinChannels(server, vector, client);
}

//prout

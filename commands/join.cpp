#include "../includes/Commands.hpp"

static void    joinChannels(Server &server, std::string &channelName, std::string &channelPassword, Client *client)
{
    std::string message;

    if (client == NULL)
    {
        std::cout << RED << "[" << "?" << "] [Server] Client is not connected to server" << RESET << std::endl;
        return ;
    }
    message = client->getNickname() + " Will join channel " + channelName + " , password = " + channelPassword + "\r\n";
    server.sendToClient(message, client->getFd());

    //Regarder si le nom du channel est existant
            // Si oui, tenter de le rejoindre.
            // Si non, le creer et ajouter client en channel operator.
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

static void parseChannels(Server &server, Client *client, std::vector<std::string> &arguments, std::vector<std::pair<std::string, std::string> > &channels)
{
    std::string name;
    std::string password;
    std::pair<std::string, std::string> channelPair;
    std::string message;

    for (size_t i = 0; i < arguments.size(); i++)
    {
        if (i == 0)
        {
            name = arguments[i];
            if (validChannelName(name) == false)
            {
                message = ERR_NOSUCHCHANNEL(client->getNickname(), name);
                server.sendToClient(message, client->getFd());
            }
        }
    }

    name = arguments[0]; // Le premier argument sera obligatoirement un nom de channel.
    if (validChannelName(name) == false)
    {
        message = ERR_NOSUCHCHANNEL(client->getNickname(), name);
        server.sendToClient(message, client->getFd());
        return ;
    }
    if (arguments.size() == 1)
        password = "";
    channelPair = std::make_pair<std::string, std::string>(name, password);
    channels.push_back(channelPair);
}

void           exec_JOIN(Server &server, std::vector<std::string> &arguments, int clientFd)
{
    Client      *client;
    std::string message;


    client = server.getClientStruct(clientFd);
    if (client == NULL)
    {
        std::cout << RED << "[" << clientFd << "] [Server] Client is not connected to server" << RESET << std::endl;
        return ;
    }

    if (arguments.size() == 0)
    {
        message = ERR_NEEDMOREPARAMS(client->getNickname(), "JOIN");
        server.sendToClient(message, clientFd);
        return ;
    }
    //parser si plusieurs channels.

    std::vector<std::pair<std::string, std::string> > channels;
    parseChannels(server, client, arguments, channels);
    if (channels.size() == 0)
    {
        std::cout << RED << "Error channel parsing, no valid channel was entered" << RESET << std::endl;
        return ;
    }

    joinChannels(server, channels[0].first, channels[0].second, client);
}

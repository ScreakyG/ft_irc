#pragma once

#include <string>

#include "Client.hpp"

class Client;

class Channel
{
    public:

        /****CONSTRUCTORS & DESTRUCTORS ****/
        Channel();
        Channel(std::string channelName, std::string channelPassword);
        Channel(const Channel &src);
        ~Channel();

        /****OPERATORS****/
        Channel& operator=(const Channel &rhs);

        /****METHODS****/

        std::string             getChannelName();
        std::string             getChannelPassword(void);
        std::string             getChannelTopic(void);
        std::vector<Client *>&   getActiveUsersVector(void);

        void                    addClient(Client *client);
        void                    quitClient(Client *client);

        void                    printUsers(void);

    private:
        std::string             _channelName;
        std::string             _channelPassword;
        std::vector<Client *>   _connectedClients;
        std::string             _topic;
};

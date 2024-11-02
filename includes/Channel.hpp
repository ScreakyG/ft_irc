#pragma once

#include <string>

#include "Client.hpp"
#include "Server.hpp"


class Client;
class Server;

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
        std::vector<Client *>&  getActiveUsersVector(void);
        std::vector<Client *>&  getActiveOperatorsVector(void);
        Client*                 getClientOnChannel(std::string name);

        void                    addClient(Client *client);
        void                    quitClient(Client *client);

        void                    addOperator(Client *client);
        void                    quitOperator(Client *client);

        bool                    isUserOperator(Client *client);

        void                    announceNewUser(Server &server, std::string &message);

        void                    setInviteMode(bool value);
        bool                    isInviteOnly(void);

        void                    setTopicRestrictions(bool value);
        bool                    isTopicRestricted(void);

        void                    printUsers(void);
        void                    printOperators(void);

    private:
        std::string             _channelName;
        std::string             _channelPassword;
        std::vector<Client *>   _connectedClients;
        std::vector<Client *>   _connectedOperators;
        std::string             _topic;
        bool                    _inviteOnly;
        bool                    _topicRestricted;
};

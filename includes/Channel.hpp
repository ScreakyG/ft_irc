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
        std::vector<Client *>&  getInvitedUsersVector(void);
        Client*                 getClientOnChannel(std::string name);
        std::string             getChannelModes(void);

        void                    addClient(Client *client);
        void                    quitClient(Client *client);

        void                    addOperator(Client *client);
        void                    quitOperator(Client *client);

        bool                    isUserOnChannel(Client *client);
        bool                    isUserOperator(Client *client);

        void                    announceNewUser(Server &server, std::string &message);

        void                    setInviteMode(bool value);
        bool                    isInviteOnly(void);

        void                    setTopicRestrictions(bool value);
        bool                    isTopicRestricted(void);
        void                    setTopic(std::string topic);

        void                    printUsers(void);
        void                    printOperators(void);

        void                    changePassword(std::string password);
        unsigned long           getUsersLimit(void);
        void                    changeUsersLimit(unsigned long limit);
        bool                    isClientOnChannel(Client *client);
        // bool                    isClientInChannel(Client* client);
        void                    broadcastMessage(const std::string& message, int excludeClientFd);
        bool                    isClientInvited(Client *client);
        void                    removeClientFromWhitelist(Client *client);

    private:
        std::string             _channelName;
        std::string             _channelPassword;
        std::vector<Client *>   _connectedClients;
        std::vector<Client *>   _connectedOperators;
        std::string             _topic;
        bool                    _inviteOnly;
        bool                    _topicRestricted;
        unsigned long           _usersLimit;
       std::vector<Client *>    _invitedUsers;
};

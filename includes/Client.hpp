#pragma once

#include <string>
#include <ctime>
#include <vector>

#include "Channel.hpp"
#include "Responses.hpp"
#include "Server.hpp"

class Channel;
class Server;

class Client
{
    public:

        /****CONSTRUCTORS & DESTRUCTORS ****/
        Client();
        Client(const Client &src);
        ~Client();

        /****OPERATORS****/
        Client& operator=(const Client &rhs);

        /****METHODS****/
        void        setFd(int fd); // Modifie _clientFd.
        int         getFd(void); // Retourne _clientFd.

        void        setNickname(std::string nickname); // Modifie _nickname.
        std::string getNickname(void); // Retourne _nickname.

        std::string getOldNickname(void); // Retourne _oldNickname.

        void        setUsername(std::string username); // Modifie _username.
        std::string getUsername(void); // Retourne _username.

        void        setHostname(std::string hostname); // Modifie _hostname.
        std::string getHostname(void); // Retourne _hostname.

        void        setRegistered(bool value); // Modifie _hasRegistered.
        bool        hasRegistered(void); // Retourne _hasRegistered.

        void        setServerPassword(bool value); // Modifie _enteredServerPassword.
        bool        hasEnteredServerPassword(void); // Retourne _enteredServerPassword.

        void        setTimeoutStart(time_t start);
        time_t      getTimeoutStart(void);

        std::string& getClientReadBuffer(void); // Retourne le _clientReadBuffer;
        void        addtoClientReadBuffer(std::string &string); // Ajoute string au _clientReadBuffer;

        std::string& getClientSendBuffer(void); // Retourne le _clientSendBuffer;
        void        addToClientSendBuffer(std::string &string); // Ajoute string au _clientSendBuffer;

        bool        alreadyJoined(std::string &channelName); // Regarde si un client est deja dans un channel.
        void        joinChannel(Server &server, Channel *channel, std::string channelPassword); // Rejoint le channel.
        void        joinAsInvited(Server &server, Channel *channel);
        void        leaveChannel(Channel *channel); //Quitte un channel
        void        leaveAllChannels(); //Quitte tout les channels.

    private:
        int                     _clientFd;
        std::string             _clientReadBuffer;
        std::string             _clientSendBuffer;
        std::string             _nickname;
        std::string             _oldNickname;
        std::string             _username;
        std::string             _hostname;
        bool                    _enteredServerPassword;
        bool                    _hasRegistered;
        time_t                  _timeoutStart;
        std::vector<Channel *>  _clientChannels;
};

#pragma once

#include <string>
#include <ctime>

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

        void        setUsername(std::string username); // Modifie _username.
        std::string getUsername(void); // Retourne _username.

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

    private:
        int         _clientFd;
        std::string _clientReadBuffer;
        std::string _clientSendBuffer;
        std::string _nickname;
        std::string _username;
        bool        _enteredServerPassword;
        bool        _hasRegistered;
        time_t      _timeoutStart;
};

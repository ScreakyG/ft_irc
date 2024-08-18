#pragma once

#include <string>

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

    private:
        int         _clientFd;
        std::string _nickname;
        std::string _username;
        bool        _hasRegistered;
};
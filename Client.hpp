#pragma once

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
        void    setFd(int fd); // Modifie _clientFd.
        int     getFd(void); // Retourne _clientFd;

    private:
        int _clientFd;
};

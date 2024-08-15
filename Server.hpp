#pragma once

#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstdlib>
#include <exception>
#include <cerrno>

#include "Random.hpp"

#define DEBUG LIGHT
#define SERVER_PORT "4242"
#define BACKLOG 10

class Server
{
    public:

        /****CONSTRUCTORS & DESTRUCTORS ****/

        Server(); //Constructor.
        Server(const Server &src); // Copy constructor.
        ~Server(); // Destructor.

        /****OPERATORS****/
        Server& operator=(const Server &rhs); // Assignement operator.

        /****METHODS****/
        void    createServerSocket(void); // Creer un socket pour le server.
        void    listenPort(void); // Ecoute sur le SERVER_PORT avec X BACKLOG.
        int     getServerSocket(void); // Retourne le socket du server.


        /****EXCEPTIONS****/
        class SocketCreationError : public std::exception
        {
            public:
                virtual const char *what(void) const throw();
        };
        class SocketBindError : public std::exception
        {
            public:
                virtual const char *what(void) const throw();

        };

    private:
        int _serverSocket;
};

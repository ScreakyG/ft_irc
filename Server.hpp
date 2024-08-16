#pragma once

#include <cstdio>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstdlib>
#include <exception>
#include <cerrno>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <vector>

#include "Random.hpp"

#define DEBUG FULL

#define SERVER_IP "127.0.0.1"
#define DEFAULT_PORT 4242

//#define BACKLOG 10

class Server
{
    public:

        /****CONSTRUCTORS & DESTRUCTORS ****/

        Server(int port); // Constructor with port parameter.
        Server(const Server &src); // Copy constructor.
        ~Server(); // Destructor.

        /****OPERATORS****/
        Server& operator=(const Server &rhs); // Assignement operator.

        /****METHODS****/
        void    serverInit(void); // Appelle les fonctions pour init.
        void    createServerSocket(void); // Creer un socket pour le server.
        void    createIpv4Address(const char *ip, int port);
        void    bindServerSocket(void); // Bind socket to address and port.
        void    listenPort(void); // Ecoute sur le SERVER_PORT avec X BACKLOG.

        int     getServerSocket(void); // Retourne le socket du server.

        void    startServerRoutine(void);
        void    acceptNewClient(void); // Accepte une nouvelle connexion d'un client.
        void    readClient(int idx); // Lit une socket client prete en lecture.


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
        class ListenServerError: public std::exception
        {
            public:
                virtual const char *what(void) const throw();
        };
        class PollError : public std::exception
        {
            public:
                virtual const char *what(void) const throw();
        };

    private:
        int                 _serverPort;
        int                 _serverSocket;
        sockaddr_in         _serverAddress;
       std::vector<pollfd>  _allSockets;

       Server(); //Default Constructor.
};

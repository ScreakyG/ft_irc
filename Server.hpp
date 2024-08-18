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
#include <signal.h>
#include <sstream>
#include <string>

#include "Random.hpp"
#include "Client.hpp"

#define DEBUG OFF

#define SERVER_IP "127.0.0.1"
#define DEFAULT_PORT 4242

//#define BACKLOG 10

class Server
{
    public:

        /****CONSTRUCTORS & DESTRUCTORS ****/
        Server(int port); // Constructor with port parameter.
        ~Server(); // Destructor.

        /****OPERATORS****/
        Server& operator=(const Server &rhs); // Assignement operator.

        /****METHODS****/
        static void     signalHandler(int signum); // Signal handler.

        void            serverInit(void); // Appelle les fonctions pour init.
        void            createServerSocket(void); // Creer un socket pour le server.
        void            createIpv4Address(const char *ip, int port);
        void            bindServerSocket(void); // Bind socket to address and port.
        void            listenPort(void); // Ecoute sur le SERVER_PORT avec X BACKLOG.

        void            startServerRoutine(void);
        void            acceptNewClient(void); // Accepte une nouvelle connexion d'un client.
        void            readClient(int idx); // Lit une socket client prete en lecture.

        void            deleteClient(int idx); //Remove from poll allSockets.
        void            closeAllFds(void); // Close all fds.

        int             getServerSocket(void); // Retourne le socket du server.
        Client&         getClientStruct(int clientFd);

        void            handleCommand(char *msg, int clientFd);
        void            registerClient(int cliendFd, std::string &commands);

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
        static bool                _sigintSignal;
        int                        _serverPort;
        int                        _serverSocket;
        sockaddr_in                _serverAddress;
        std::vector<pollfd>        _allSockets;
        std::vector<Client>        _allClients;

       Server(); //Default Constructor.
       Server(const Server &src); // Copy constructor.
};

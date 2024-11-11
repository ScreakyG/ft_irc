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
#include <algorithm>
#include <sstream>
#include <fcntl.h>
#include <string>


#include "Random.hpp"
#include "Client.hpp"
#include "Commands.hpp"
#include "Colors.hpp"
#include "Responses.hpp"
#include "Channel.hpp"

#define DEBUG LIGHT

//#define SERVER_NAME "localhost"
#define SERVER_IP "127.0.0.1"
#define DEFAULT_PORT 4242
#define DEFAULT_PASSWORD "lol"

#define NICK_MAXLEN 9
#define REGISTER_TIMEOUT 30

//#define BACKLOG 10

class Channel;
class Client;

class Server
{
    public:

        /****CONSTRUCTORS & DESTRUCTORS ****/
        Server(int port, std::string password); // Constructor with port parameter and password.
        ~Server(); // Destructor.

        /****OPERATORS****/
        Server& operator=(const Server &rhs); // Assignement operator.

        /****METHODS****/
        static void             signalHandler(int signum); // Signal handler.

        void                    serverInit(void); // Appelle les fonctions pour init.
        void                    createServerSocket(void); // Creer un socket pour le server.
        void                    createIpv4Address(const char *ip, int port);
        void                    bindServerSocket(void); // Bind socket to address and port.
        void                    listenPort(void); // Ecoute sur le SERVER_PORT avec X BACKLOG.

        void                    startServerRoutine(void);
        void                    acceptNewClient(void); // Accepte une nouvelle connexion d'un client.
        void                    readClient(int idx); // Lit une socket client prete en lecture.
        void                    checkClientSendBuffer(int idx); // Regarde si il y a des choses dans le sendBuffer du client , et dans ce cas essaye de lui envoyer.
        void                    sendToClient(std::string &message, int clientFd); // Envoie un message client.

        void                    deleteClient(int idx); //Remove from poll allSockets.
        void                    deleteClientFromAllWhitelists(Client *client);
        void                    closeAllFds(void); // Close all fds.
        void                    deleteAllChannels(void);
        void                    deleteAllClients(void);

        int                     getServerSocket(void); // Retourne le socket du server.
        Client*                 getClientStruct(int clientFd);
        pollfd*                 getClientPoll(int clientFd);
        std::string             getServerPassword(void); // Retourne le mot de passe du serveur.
        std::vector<Client *>&  getVectorClient(void); // Retourne le vecteur de client.
        Client*                 getClientByName(std::string clientName); //Retoure un client grace a son username.

        void                    handleMessage(char *buffer, int clientFd);
        void                    handleCommand(std::string &command, int clientFd);
        void                    executeCommand(std::string &ogString, std::string &commandName, std::vector<std::string> &arguments, int clientFd);

        void                    isRegistrationComplete(Client *client); // Regarde si la registration est complete.

        void                    checkClientRegisterTimeouts();

        bool                    channelExist(std::string name); // Regarde dans le vecteur '_Channels' si il y en a un qui existe avec le meme nom.
        void                    addChannel(Channel *newChannel); // Ajoute un channel dans le vecteur '_Channels'.
        Channel*                getChannel(std::string channelName); // Retourne le channel du vecteur '_Channels' correspondant a channelName.


        void                    printAllUsers(void);

        void                    sendReply(int clientFd, int errorCode, const std::string& message);
        Client*                 getClientByNickname(const std::string& nickname);

        std::vector<Client *>& getAllClients(void) { return _allClients; }

        size_t getNbClients(void) const { return _allClients.size(); }

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
        class ClientDisconnect : public std::exception
        {
            public:
                virtual const char *what(void) const throw();
        };

    private:
        static bool                _stopSignal;
        long                       _serverPort;
        std::string                _serverPassword;
        int                        _serverSocket;
        sockaddr_in                _serverAddress;
        std::vector<pollfd>        _allSockets;
        std::vector<Client *>      _allClients;
        std::vector<Channel *>     _Channels;

       Server(); //Default Constructor.
       Server(const Server &src); // Copy constructor.
};

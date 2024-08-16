#include "Server.hpp"
#include "Colors.hpp"

/******************************/
/***CONSTRUCTORS/DESTRUCTORS***/
/******************************/

Server::Server(void): _serverSocket(-1)
{
    if (DEBUG == FULL)
        std::cout << "Server default constructor called." << std::endl;
}

Server::Server(const Server &src): _serverSocket(src._serverSocket)
{
    if (DEBUG == FULL)
        std::cout << "Server copy constructor called." << std::endl;
}

Server::~Server(void)
{
    if (DEBUG == FULL)
        std::cout << "Server destructor called." << std::endl;

}

/******************************/
/***********OPERATORS**********/
/******************************/

Server& Server::operator=(const Server &rhs)
{
    if (this != &rhs)
    {
        if (DEBUG == FULL)
            std::cout << "Server assignement operator success." << std::endl;
        this->_serverSocket = rhs._serverSocket;
    }
    else
        if (DEBUG == FULL)
        std::cout << RED << "Server assignement operator failed." << RESET << std::endl;

    return (*this);
}

/******************************/
/***********METHODS************/
/******************************/

void Server::createServerSocket(void)
{
    int         status;
    addrinfo    hint;
    addrinfo    *res;

    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC; // IP CAN BE IPV4 OR IPV6.
    hint.ai_socktype = SOCK_STREAM; // USE TCP.
    hint.ai_flags = AI_PASSIVE; // Trouver pourquoi ?

    status = getaddrinfo(NULL, SERVER_PORT, &hint, &res);
    if (status != 0)
        throw Server::SocketCreationError();
    this->_serverSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol); // Protocole peut etre mis a 0.

    if (DEBUG == LIGHT || DEBUG == FULL)
        std::cout << PURPLE << "Created server socket = " << this->_serverSocket << RESET << std::endl;

    status = bind(this->_serverSocket, res->ai_addr, res->ai_addrlen);
    if (status != 0)
        throw Server::SocketBindError();

    if (DEBUG == LIGHT || DEBUG == FULL)
        std::cout << PURPLE << "Server socket bound to localhost on port : " << SERVER_PORT << RESET << std::endl;
}

void Server::listenPort(void)
{
    listen(this->_serverSocket, BACKLOG);
        if (DEBUG == LIGHT)
            std::cout << PURPLE << "Server is listening on port " << SERVER_PORT << "..." << RESET << std::endl;
}

int Server::getServerSocket(void)
{
    return (this->_serverSocket);
}

/******************************/
/*********EXCEPTIONS***********/
/******************************/

const char* Server::SocketCreationError::what(void) const throw()
{
    std::cout << RED << "Error while creating server socket : " << RESET;
    return (std::strerror(errno));
}


const char* Server::SocketBindError::what(void) const throw()
{
    std::cout  << RED << "Error while binding server socket : " << RESET;
    return (std::strerror(errno));
}

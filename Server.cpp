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
    this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0); // IPV4, TCP , protocole auto.
    if (this->_serverSocket == -1)
        throw Server::SocketCreationError();
    if (DEBUG == LIGHT || DEBUG == FULL)
        std::cout << PURPLE << "Created server socket = " << this->_serverSocket << RESET << std::endl;
}

void Server::createIpv4Address(const char *ip, int port)
{
    this->_serverAdress.sin_family = AF_INET; // Ipv4.
    this->_serverAdress.sin_port = htons(port);
    inet_pton(AF_INET, ip, &_serverAdress.sin_addr.s_addr); // Transform ipv4/ipv6 addresses from test to binary.
}

void Server::bindServerSocket(void)
{
    int status;

    status = bind(this->_serverSocket, reinterpret_cast<sockaddr*>(&this->_serverAdress), sizeof(this->_serverAdress));
    if (status != 0)
        throw Server::SocketBindError();

    if (DEBUG == LIGHT || DEBUG == FULL)
        std::cout << PURPLE << "Server socket bound to localhost on port : " << SERVER_PORT << RESET << std::endl;
}

void Server::listenPort(void)
{
    int status;

    status = listen(this->_serverSocket, BACKLOG);
    if (status != 0)
        throw Server::ListenServerError();
    if (DEBUG == LIGHT || DEBUG == FULL)
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
const char* Server::ListenServerError::what(void) const throw()
{
    std::cout  << RED << "Error while listening server socket : " << RESET;
    return (std::strerror(errno));
}

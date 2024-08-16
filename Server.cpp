#include "Server.hpp"
#include "Colors.hpp"

/******************************/
/***CONSTRUCTORS/DESTRUCTORS***/
/******************************/

Server::Server(void): _serverSocket(-1)
{
    if (DEBUG == FULL)
        std::cout << "Server default constructor called." << std::endl;

    std::memset(&_serverAddress, 0, sizeof(_serverAddress));
}

Server::Server(const Server &src): _serverSocket(src._serverSocket), _serverAddress(src._serverAddress)
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
        this->_serverAddress = rhs._serverAddress;
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
        std::cout << PURPLE << "Created server socket fd = " << this->_serverSocket << RESET << std::endl;
}

void Server::createIpv4Address(const char *ip, int port)
{
    this->_serverAddress.sin_family = AF_INET; // Ipv4.
    this->_serverAddress.sin_port = htons(port); // Converti le port int en quelque chose comprehensible pour la machine.
    inet_pton(AF_INET, ip, &(this->_serverAddress.sin_addr)); // Transform ipv4/ipv6 addresses from test to binary.
}

void Server::bindServerSocket(void)
{
    int status;

    status = bind(this->_serverSocket, reinterpret_cast<sockaddr*>(&this->_serverAddress), sizeof(this->_serverAddress));
    if (status != 0)
        throw Server::SocketBindError();
    if (DEBUG == LIGHT || DEBUG == FULL)
        std::cout << PURPLE << "Server socket bound to " << SERVER_IP << " on port : " << SERVER_PORT << RESET << std::endl;
}

void Server::listenPort(void)
{
    int status;

    status = listen(this->_serverSocket, SOMAXCONN); // SOMAXCONN = max de connexions dispo sur la machine.
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
    std::cerr << RED << "Error while creating server socket : " << RESET;
    return (std::strerror(errno));
}


const char* Server::SocketBindError::what(void) const throw()
{
    std::cerr  << RED << "Error while binding server socket : " << RESET;
    return (std::strerror(errno));
}

const char* Server::ListenServerError::what(void) const throw()
{
    std::cerr  << RED << "Error while listening server socket : " << RESET;
    return (std::strerror(errno));
}

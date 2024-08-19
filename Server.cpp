#include "includes/Server.hpp"

/******************************/
/***CONSTRUCTORS/DESTRUCTORS***/
/******************************/

bool Server::_stopSignal = false;

Server::Server(void): _serverPort(DEFAULT_PORT), _serverPassword(DEFAULT_PASSWORD), _serverSocket(-1)
{
    if (DEBUG == FULL)
        std::cout << "Server default constructor called." << std::endl;

    std::memset(&_serverAddress, 0, sizeof(_serverAddress));
}

Server::Server(int port, std::string password): _serverPort(port), _serverPassword(password), _serverSocket(-1)
{
    if (DEBUG == FULL)
        std::cout << "Server port constructor called." << std::endl;

    std::memset(&_serverAddress, 0, sizeof(_serverAddress));
}

Server::Server(const Server &src): _serverPort(src._serverPort), _serverPassword(src._serverPassword), _serverSocket(src._serverSocket), _serverAddress(src._serverAddress), _allSockets(src._allSockets), _allClients(src._allClients)
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
        this->_serverPort = rhs._serverPort;
        this->_serverPassword = rhs._serverPassword;
        this->_serverSocket = rhs._serverSocket;
        this->_serverAddress = rhs._serverAddress;
        this->_allSockets = rhs._allSockets;
        this->_allClients = rhs._allClients;
    }
    else
        if (DEBUG == FULL)
        std::cout << RED << "Server assignement operator failed." << RESET << std::endl;

    return (*this);
}

/******************************/
/***********METHODS************/
/******************************/

void Server::signalHandler(int signum)
{
    std::cout << "Caught signal : " << signum << std::endl;
    Server::_stopSignal = true;
    std::cout << RED << "Closing server." << RESET << std::endl;
}

void Server::serverInit(void)
{
    createServerSocket();
    createIpv4Address(SERVER_IP, this->_serverPort);
    bindServerSocket();
    listenPort();
}

void Server::createServerSocket(void)
{
    this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0); // IPV4, TCP , protocole auto.
    if (this->_serverSocket == -1)
        throw Server::SocketCreationError();

    if (DEBUG == LIGHT || DEBUG == FULL)
        std::cout << PURPLE << "Created server socket fd = " << this->_serverSocket << RESET << std::endl;

    // Permet de reutiliser l'adresse du serveur sans delai, par exemple lorsque l'on relance le serveur il y a un delai par moment.
    int activate = 1;
    if (setsockopt(this->_serverSocket, SOL_SOCKET, SO_REUSEADDR, &activate, sizeof(activate)) == -1)
        throw std::runtime_error("Failed to set SO_REUSEADDR option to socket");
}

void Server::createIpv4Address(const char *ip, int port)
{
    this->_serverAddress.sin_family = AF_INET; // Ipv4.
    this->_serverAddress.sin_port = htons(port); // Converti le port int en quelque chose comprehensible pour la machine.
    inet_pton(AF_INET, ip, &(this->_serverAddress.sin_addr)); // Transform ipv4/ipv6 addresses from test to binary. Peut etre changer en INADDR_ANY pour signifier n'importe quelle interface dispo.
}

void Server::bindServerSocket(void)
{
    int status;

    status = bind(this->_serverSocket, reinterpret_cast<sockaddr*>(&this->_serverAddress), sizeof(this->_serverAddress));
    if (status != 0)
        throw Server::SocketBindError();

    if (DEBUG == LIGHT || DEBUG == FULL)
        std::cout << PURPLE << "Server socket bound to " << SERVER_IP << " on port : " << this->_serverPort << RESET << std::endl;
}

void Server::listenPort(void)
{
    int status;

    status = listen(this->_serverSocket, SOMAXCONN); // SOMAXCONN = max de connexions dispo sur la machine.
    if (status != 0)
        throw Server::ListenServerError();

    if (DEBUG == LIGHT || DEBUG == FULL)
        std::cout << PURPLE << "Server is listening on port " << this->_serverPort << "..." << RESET << std::endl;
}

void Server::startServerRoutine(void)
{
    pollfd serverPoll;

    serverPoll.fd = this->_serverSocket;
    serverPoll.events = POLLIN;
    serverPoll.revents = 0;

    this->_allSockets.push_back(serverPoll);

    if (DEBUG == LIGHT)
        std::cout << PURPLE << "[Server] Set up poll fd array." << RESET << std::endl;

    while (Server::_stopSignal == false)
    {
        int status;

        checkClientTimeouts();

        status = poll(&this->_allSockets[0], this->_allSockets.size(), 2000);
        if (Server::_stopSignal == true)
            break ;
        if (status == -1)
            throw Server::PollError();
        if (status == 0)
        {
            if (DEBUG == FULL)
                std::cout << PURPLE << "[Server] Waiting..." << RESET << std::endl;
            continue ;
        }
        for (size_t i = 0; i < this->_allSockets.size(); i++)
        {
            if (this->_allSockets[i].revents & POLLIN)
            {
                if (this->_allSockets[i].fd == this->_serverSocket)
                    acceptNewClient();
                else
                    readClient(i);
            }
        }
    }
}

void Server::acceptNewClient(void)
{
    // Penser a parse les infos de connexions style IP du client.
    pollfd  newClientPoll;
    Client  newClientStruct;
    int     newClientFd;
    time_t  timeoutStart;

    newClientFd = accept(this->_serverSocket, NULL, NULL); // Options a peut etre revoir.
    if (newClientFd == -1)
        std::cerr << "[Server] Couldn't connect new client." << std::endl;
    else
    {
        newClientPoll.fd = newClientFd;
        newClientPoll.events = POLLIN;
        newClientPoll.revents = 0;

        newClientStruct.setFd(newClientFd);

        timeoutStart = time(NULL);
        newClientStruct.setTimeoutStart(timeoutStart);

        this->_allSockets.push_back(newClientPoll);
        this->_allClients.push_back(newClientStruct);

        std::cout << GREEN << "[Server] New client connected on fd : " << newClientFd << RESET << std::endl;
    }
}

void Server::readClient(int idx)
{
    int     amountReceived;
    char    buffer[BUFSIZ];

    memset(buffer, '\0', sizeof(buffer));
    amountReceived = recv(this->_allSockets[idx].fd, buffer, BUFSIZ - 1, 0); // BUFSIZ - 1 pour garder minimum un espace pour le '\0'.

    if (amountReceived <= 0)
    {
        std::cout << "[" << this->_allSockets[idx].fd << "]" << " : Closed connection." << std::endl;
        deleteClient(this->_allSockets[idx].fd);
    }
    else
    {
        buffer[amountReceived] = '\0';
        std::cout << GREEN << "[" << this->_allSockets[idx].fd << "] [Client] : " << buffer << RESET;
        handleMessage(buffer, this->_allSockets[idx].fd);
    }
}

void Server::handleMessage(char *buffer, int clientFd)
{
    std::istringstream  msgFromClient(buffer);
    std::string         line;

    while (std::getline(msgFromClient, line))
        handleCommand(line, clientFd);
}

void Server::handleCommand(std::string &command, int clientFd)
{
    std::istringstream          iss(command);
    std::string                 commandName;
    std::string                 argument;
    std::vector<std::string>    argumentsVector;

    iss >> commandName; // Premier mot de la ligne est la commande.
    while (iss >> argument) // Lire le reste qui correspond aux arguments.
        argumentsVector.push_back(argument);

    if (DEBUG == LIGHT || DEBUG == FULL)
    {
        std::cout << CYAN << "[" << clientFd << "] " << "[Server] Command = " << command << RESET << std::endl;

        std::cout << YELLOW << "[" << clientFd << "] " << "[Server] COMMAND = " << commandName << RESET;
        for(size_t i = 0; i < argumentsVector.size(); i++)
        {
            std::cout << YELLOW << " | ARG_" << i << " = " << RESET;
            std::cout << YELLOW << argumentsVector[i] << " " << RESET;
        }
        std::cout << std::endl;
    }
    executeCommand(commandName, argumentsVector, clientFd);
}

void Server::executeCommand(std::string &commandName, std::vector<std::string> &arguments, int clientFd)
{
    std::string message;

    if (commandName == "CAP")
        return ;
    else if (commandName == "PASS")
        exec_PASS((*this), arguments, clientFd);
    else if (commandName == "NICK")
        exec_NICK((*this), arguments, clientFd);
    else if (commandName == "USER")
        exec_USER((*this), arguments, clientFd);
    else
    {
        message = std::string("421 ") + commandName + " :Unknown command\n";
        sendToClient(message, clientFd);
    }
}

void Server::isRegistrationComplete(Client *client)
{
    std::string message;

    if (client->getNickname() != "" && client->getUsername() != "") // Cela veut dire que elle ne sont plus a default et on ete modifie.
    {
        message = std::string("001 ") + client->getNickname() + " :Welcome to the Internet Relay Network : " + client->getUsername() + "\n";
        client->setRegistered(true);
        sendToClient(message, client->getFd());

        return ;
    }
}

void Server::checkClientTimeouts(void)
{
    std::string message;

    for (size_t i = 0; i < _allClients.size(); i++)
    {
        if (_allClients[i].hasRegistered() == false)
        {
            if (difftime(time(NULL), _allClients[i].getTimeoutStart()) > REGISTER_TIMEOUT)
            {
                message = "ERROR :Registration timeout\n";
                sendToClient(message, _allClients[i].getFd());
                deleteClient(_allClients[i].getFd());
            }
        }
    }
}

Client* Server::getClientStruct(int clientFd)
{
    for (size_t i = 0; i < this->_allClients.size(); i++)
    {
        if (this->_allClients[i].getFd() == clientFd)
            return (&this->_allClients[i]);
    }
    return (NULL); //En theorie cela ne devrai jamais arriver.
}

std::vector<Client>&    Server::getVectorClient(void)
{
    return (this->_allClients);
}

int Server::getServerSocket(void)
{
    return (this->_serverSocket);
}

std::string Server::getServerPassword(void)
{
    return (this->_serverPassword);
}

void Server::deleteClient(int fd_toClear)
{
    for (size_t i = 0; i < this->_allSockets.size(); i++)  // Remove it from the '_pollfd allSockets';
    {
        if (this->_allSockets[i].fd == fd_toClear)
        {
            this->_allSockets.erase(this->_allSockets.begin() + i);
            break ;
        }
    }

    for (size_t i = 0; i < this->_allClients.size(); i++) // Remove it from the '_Client allClient';
    {
        if (this->_allClients[i].getFd() == fd_toClear)
        {
            this->_allClients.erase(this->_allClients.begin() + i);
            break ;
        }
    }
     close(fd_toClear); // Close the socket
     std::cout << "Disconnecting socket : [" << fd_toClear  << "]" << std::endl;
}

void Server::closeAllFds(void)
{
    for (size_t i = 0; i < this->_allSockets.size(); i++)
    {
        if (this->_allSockets[i].fd != this->_serverSocket)
            std::cout << "Disconnecting socket : [" << this->_allSockets[i].fd  << "]" << std::endl;
        close(this->_allSockets[i].fd);
    }
}

bool  Server::clientValidPassword(Client *client, int clientFd)
{
    std::string message;
    if (client->hasEnteredServerPassword() == false)
    {
        message = "451 : Password required before registering\n";
        sendToClient(message, clientFd);
        return (false);
    }
    return (true);
}

void Server::sendToClient(std::string &message, int clientFd)
{
    ssize_t sentBytes = -1;

    std::cout << PURPLE << "[Server] ---> " << "[" << clientFd << "] : " << message << RESET;

    sentBytes = send(clientFd, message.c_str(), message.size(), 0);
    if (sentBytes == -1)
        std::cerr << RED << "[Server] " << "[" << clientFd << "] " << "sending error : " << std::strerror(errno) << RESET << std::endl;
    else if (static_cast<size_t>(sentBytes) != message.size())
        std::cout << YELLOW << "[Server] " << "[" << clientFd << "] " <<"Partial message was sent to client." << RESET << std::endl;
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
    std::cerr << RED << "Error while binding server socket : " << RESET;
    return (std::strerror(errno));
}

const char* Server::ListenServerError::what(void) const throw()
{
    std::cerr << RED << "Error while listening server socket : " << RESET;
    return (std::strerror(errno));
}

const char* Server::PollError::what(void) const throw()
{
    std::cerr << RED << "Poll function failed : " << RESET;
    return (std::strerror(errno));
}

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

Server::Server(const Server &src): _serverPort(src._serverPort), _serverPassword(src._serverPassword), _serverSocket(src._serverSocket), _serverAddress(src._serverAddress), _allSockets(src._allSockets), _allClients(src._allClients), _Channels(src._Channels)
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
        this->_Channels = rhs._Channels;
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
    {
        close(this->_serverSocket);
        throw std::runtime_error("Failed to set SO_REUSEADDR option to socket");
    }
    int flags = fcntl(this->_serverSocket, F_GETFL, 0); //Recupere les flags actuels de la socket.
    if (fcntl(this->_serverSocket, F_SETFL, flags | O_NONBLOCK) == -1) // Set des nouveaux flags sur la socket, les ancients + O_NONBLOCK.
    {
        close(this->_serverSocket);
        throw std::runtime_error("Failed to set O_NONBLOCK option to socket");
    }
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
    {
        close(this->_serverSocket);
        throw Server::SocketBindError();
    }

    if (DEBUG == LIGHT || DEBUG == FULL)
        std::cout << PURPLE << "Server socket bound to " << SERVER_IP << " on port : " << this->_serverPort << RESET << std::endl;
}

void Server::listenPort(void)
{
    int status;

    status = listen(this->_serverSocket, SOMAXCONN); // SOMAXCONN = max de connexions dispo sur la machine.
    if (status != 0)
    {
        close(this->_serverSocket);
        throw Server::ListenServerError();
    }
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

        checkClientRegisterTimeouts();

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
        for (size_t i = 0; i < this->_allSockets.size(); i++) // Faire une deuxieme condition et regarder POLLOUT.
        {
            if (this->_allSockets[i].revents & POLLIN)
            {
                if (this->_allSockets[i].fd == this->_serverSocket)
                    acceptNewClient();
                else
                    readClient(i);
            }
            else if (this->_allSockets[i].revents & POLLOUT) // Peu poser probleme si readClient a ferme la connexion avant. else if empeche les deux de se faire dans le meme tour.
                checkClientSendBuffer(i);
        }
    }
}

void Server::checkClientSendBuffer(int idx)
{
    Client*     clientStruct;
    std::string message = "";

    clientStruct = getClientStruct(_allSockets[idx].fd);
    if (clientStruct == NULL)
    {
        std::cout << RED << "[" << "?" << "] [Server] Client is not connected to server" << RESET << std::endl;
        return ;
    }
    try
    {
        if (clientStruct->getClientSendBuffer().empty() == false)
        {
            std::cout << YELLOW << "[" << clientStruct->getFd() << "] [Server] Trying to send remaining buffer to client" << RESET << std::endl;
            sendToClient(message, clientStruct->getFd());
        }
    }
    catch(Server::ClientDisconnect &e)
    {
        std::cout << e.what() << std::endl;
        std::cout << RED << "Error trying to resend buffer to client" << RESET << std::endl;
        deleteClient(clientStruct->getFd());
    }
}

void Server::acceptNewClient(void)
{
    // Penser a parse les infos de connexions style IP du client.
    pollfd      newClientPoll;
    Client      *newClientStruct = NULL;
    int         newClientFd;
    sockaddr_in clientAdress;
    socklen_t   addr_len = sizeof(clientAdress);
    time_t      timeoutStart;

    newClientFd = accept(this->_serverSocket, reinterpret_cast<sockaddr*>(&clientAdress), &addr_len); // Options a peut etre revoir.
    if (newClientFd == -1)
    {
        std::cerr << "[Server] Couldn't connect new client." << std::endl;
        return ;
    }

    int flags = fcntl(newClientFd, F_GETFL, 0);
    if (fcntl(newClientFd, F_SETFL, flags | O_NONBLOCK) == -1) // Set des nouveaux flags sur la socket, les ancients + O_NONBLOCK.
    {
        close(newClientFd);
        std::cout << "[Server] Failed to set O_NONBLOCK option to socket" << std::endl;
        return ;
    }
    else
    {
        newClientPoll.fd = newClientFd;
        newClientPoll.events = POLLIN | POLLOUT; // Rajouter POLLOUT Ulterieurement pour egalement surveiller si la socket est prete a recevoir. Peut etre utile dans le cas de CTRL + Z.
        newClientPoll.revents = 0;

        newClientStruct = new Client();
        if (newClientStruct != NULL)
        {
            newClientStruct->setFd(newClientFd);
            timeoutStart = time(NULL);
            newClientStruct->setTimeoutStart(timeoutStart);

            getsockname(newClientFd, reinterpret_cast<sockaddr*>(&clientAdress), &addr_len);
            newClientStruct->setHostname(inet_ntoa(clientAdress.sin_addr));

            this->_allSockets.push_back(newClientPoll);
            this->_allClients.push_back(newClientStruct);

            std::cout << GREEN << "[Server] New client connected on fd : " << newClientFd << RESET << std::endl;
        }
        else
        {
            close(newClientFd);
            std::cout << "[Server] allocate memory for client" << std::endl;
        }
    }
}

void Server::readClient(int idx)
{
    int     amountReceived;
    char    buffer[BUFSIZ];

    memset(buffer, '\0', sizeof(buffer));
    amountReceived = recv(this->_allSockets[idx].fd, buffer, BUFSIZ - 1, 0); // BUFSIZ - 1 pour garder minimum un espace pour le '\0'.

    if (amountReceived == -1)
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN) // il n'y as pas de donnees pour l'instant.
            return;
        else
        {
            std::cerr << RED << "[Server] " << "[" << this->_allSockets[idx].fd << "] " << "recv error : " << std::strerror(errno) << RESET << std::endl;
            deleteClient(this->_allSockets[idx].fd);
            return;
        }

    }
    if (amountReceived == 0)
    {
        std::cout << "[" << this->_allSockets[idx].fd << "]" << " : Client closed connection." << std::endl;
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
    if (!buffer) // N'est pas sense arriver mais extra protection.
       return ;

    std::string stringedBuffer(buffer);
    Client* client;

    client = getClientStruct(clientFd);
    if (client == NULL)
    {
        std::cout << RED << "[" << clientFd << "] [Server] Client is not connected to server" << RESET << std::endl;
        return ;
    }

    client->addtoClientReadBuffer(stringedBuffer);

    std::string &readClientBuffer = client->getClientReadBuffer();
    std::string line;
    size_t pos;

    try
    {
        while ((pos = readClientBuffer.find("\r\n")) != std::string::npos) // Modifier en "\n" pour utiliser avec nc plus facilement.
        {
            line = readClientBuffer.substr(0, pos);
            handleCommand(line, clientFd);
            readClientBuffer.erase(0, pos + 2); // Si on enleve le "\r" il faut modifier en pos + 1;
        }
    }
    catch(Server::ClientDisconnect &e)
    {
        std::cout << e.what() << std::endl;
        deleteClient(client->getFd());
        // Ici on catch les disconnectClients et on les deleteClients(), cela permet de ne pas traiter le reste des commandes si le client se fait deconnecter.
    }
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
    executeCommand(command, commandName, argumentsVector, clientFd);
}

void Server::executeCommand(std::string &ogString, std::string &commandName, std::vector<std::string> &arguments, int clientFd)
{
    std::string message;
    Client *client;

    client = getClientStruct(clientFd);
    if (client == NULL)
    {
        std::cout << RED << "[" << clientFd << "] [Server] Client is not connected to server" << RESET << std::endl;
        return ;
    }

    if (commandName == "CAP")
        exec_CAP((*this), arguments, clientFd);
    else if (commandName == "PASS")
        exec_PASS((*this), arguments, clientFd);
    else if (commandName == "NICK")
        exec_NICK((*this), arguments, clientFd);
    else if (commandName == "USER")
        exec_USER((*this), arguments, clientFd);
    else if (commandName == "PING")
        exec_PING((*this), arguments, clientFd);
    else if (commandName == "JOIN")
        exec_JOIN((*this), ogString, arguments, clientFd);
    else if(commandName == "PRIVMSG")
        exec_PRIVMSG((*this), arguments, clientFd);
    else
    {
        message = ERR_UNKNOWNCOMMAND(client->getNickname(), commandName);
        sendToClient(message, clientFd);
    }
}

void Server::isRegistrationComplete(Client *client)
{
    std::string message;

    if (client == NULL)
    {
        std::cout << RED << "[" << "?" << "] [Server] Client is not connected to server" << RESET << std::endl;
        return ;
    }

   // if (client->getNickname() != "" && client->getUsername() != "" && client->hasEnteredServerPassword() == true) // Cela veut dire que elle ne sont plus a default et on ete modifie.
    if (client->getNickname().empty() == false && client->getUsername().empty() == false && client->hasEnteredServerPassword() == true) // Cela veut dire que elle ne sont plus a default et on ete modifie.
    {
        client->setRegistered(true);

        message = RPL_WELCOME(client->getNickname(), client->getNickname());
        sendToClient(message, client->getFd());
        message = RPL_YOURHOST(client->getNickname());
        sendToClient(message, client->getFd());
        message = RPL_CREATED(client->getNickname());
        sendToClient(message, client->getFd());

        return ;
    }
}

void Server::checkClientRegisterTimeouts(void)
{
    std::string message;

    for (size_t i = 0; i < _allClients.size(); i++)
    {
        if (_allClients[i]->hasRegistered() == false)
        {
            if (difftime(time(NULL), _allClients[i]->getTimeoutStart()) > REGISTER_TIMEOUT)
            {
                try
                {
                    message = ERROR_MSG(std::string("Registration timeout, PASS, NICK or USER might be incorrect"));
                    sendToClient(message, _allClients[i]->getFd()); // sendToClient peut throw en cas de probleme majeur alors on catch pour eviter un segfault.
                    deleteClient(_allClients[i]->getFd());
                }
                catch(Server::ClientDisconnect &e)
                {
                    std::cout << e.what() << std::endl;
                    deleteClient(_allClients[i]->getFd());
                }
            }
        }
    }
}

Client* Server::getClientStruct(int clientFd)
{
    for (size_t i = 0; i < this->_allClients.size(); i++)
    {
        if (this->_allClients[i]->getFd() == clientFd)
            return (this->_allClients[i]);
    }
    return (NULL); //En theorie cela ne devrai jamais arriver.
}

pollfd* Server::getClientPoll(int clientFd)
{
    for (size_t i = 0;  i < this->_allSockets.size(); i++)
    {
        if (this->_allSockets[i].fd == clientFd)
            return (&this->_allSockets[i]);
    }
    return (NULL);
}

std::vector<Client *>&    Server::getVectorClient(void)
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
    Client *client;

    client = getClientStruct(fd_toClear);
    if (!client)
        return ;


    if (client)
        client->leaveAllChannels();

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
        if (this->_allClients[i]->getFd() == fd_toClear)
        {
            Client *clientToClear = NULL;

            clientToClear = this->_allClients[i];
            this->_allClients.erase(this->_allClients.begin() + i);
            delete clientToClear;
            break ;
        }
    }
     close(fd_toClear); // Close the socket
     std::cout << "Disconnecting socket : [" << fd_toClear  << "]" << std::endl;
}

void Server::deleteAllClients(void)
{
    if (_allClients.size() > 0)
    {
        for (size_t i = 0; i < _allClients.size();)
        deleteClient(_allClients[i]->getFd());
    }
    _allClients.clear();
}

void Server::closeAllFds(void)
{
    for (size_t i = 0; i < this->_allSockets.size(); i++)
    {
        if (this->_allSockets[i].fd == this->_serverSocket)
        {
            std::cout << "Deleting server socket" << std::endl;
            close(this->_allSockets[i].fd);
        }
    }
    _allSockets.clear();

    // for (size_t i = 0; i < this->_allSockets.size(); i++)
    // {
    //     if (this->_allSockets[i].fd != this->_serverSocket)
    //         std::cout << "Disconnecting socket : [" << this->_allSockets[i].fd  << "]" << std::endl;
    //     close(this->_allSockets[i].fd);
    // }
}

void Server::sendToClient(std::string &message, int clientFd)
{
    pollfd  *client;
    Client  *clientStruct;
    ssize_t sentBytes = -1;

    client = getClientPoll(clientFd);
    clientStruct = getClientStruct(clientFd);
    if (client == NULL || clientStruct == NULL)
    {
        std::cout << RED << "[" << clientFd << "] [Server] Could not send, client is not connected to server" << RESET << std::endl;
        return ;
    }

    clientStruct->addToClientSendBuffer(message);
    if (!(client->revents & POLLOUT))
        return ;

    std::cout << PURPLE << "[Server] ---> " << "[" << clientFd << "] : " << clientStruct->getClientSendBuffer() << RESET;

    sentBytes = send(clientFd, clientStruct->getClientSendBuffer().c_str(), clientStruct->getClientSendBuffer().size(), 0);
    if (sentBytes == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK) // On garde le buffer intact et on ressaye au prochain appel de sendToClient().
            std::cerr << RED << "[Server] " << "[" << clientFd << "] " << "Sending socket temporarily unavailable for writing : " << std::strerror(errno) << RESET << std::endl;
        else // Une erreur plus grave est arrive , pour des mesures de securite il vaut mieux deconnecter le client.
        {
            std::cerr << RED << "[Server] " << "[" << clientFd << "] " << "sending error : " << std::strerror(errno) << RESET << std::endl;
            throw Server::ClientDisconnect(); // Il faut mette des try catch autour des fonctions qui sendToClient().
        }
        return ;
    }
    if (static_cast<size_t>(sentBytes) != clientStruct->getClientSendBuffer().size())
    {
        std::cout << YELLOW << "[Server] " << "[" << clientFd << "] " <<"Partial message was sent to client." << RESET << std::endl;
        clientStruct->getClientSendBuffer().erase(0, sentBytes); // Suprime le nombre de donnees que on a reussi a envoyer.
        return ;
    }
    else if (static_cast<size_t>(sentBytes) == clientStruct->getClientSendBuffer().size())
        clientStruct->getClientSendBuffer().clear(); // Si on arrive ici c'est que on a reussi a tout envoyer, alors on reset le buffer.
}

/******************************/
/*********CHANNELS***********/
/******************************/

bool Server::channelExist(std::string name)
{
    for (size_t idx = 0; idx < _Channels.size(); idx++)
    {
        if (_Channels[idx]->getChannelName() == name)
            return (true);
    }
    return (false);
}

void Server::addChannel(Channel *newChannel)
{
    if (DEBUG == LIGHT || DEBUG == FULL)
        std::cout << "NEW CHANNEL CREATED, Name = " << newChannel->getChannelName() << " | Password = " << newChannel->getChannelPassword() << std::endl;

    _Channels.push_back(newChannel);
}

Channel* Server::getChannel(std::string channelName)
{
    for (size_t idx = 0; idx < _Channels.size(); idx++)
    {
        if (_Channels[idx]->getChannelName() == channelName)
            return (_Channels[idx]);
    }
    return (NULL);
}


void Server::printAllUsers(void)
{
   std::vector<Client *>::iterator    it;

   for (it = _allClients.begin(); it != _allClients.end(); it++)
   {
        std::cout << "[Server] Nickname : " <<  (*it)->getNickname() << " | ";
        std::cout << "fd : " << (*it)->getFd();
        std::cout << std::endl;
   }
}

void Server::deleteAllChannels(void)
{
    std::vector<Channel *>::iterator    it;

    for (it = _Channels.begin(); it != _Channels.end(); it++)
    {
        (*it)->getActiveOperatorsVector().clear();
        (*it)->getActiveUsersVector().clear();
        delete(*it);
    }
    _Channels.clear();
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
const char* Server::ClientDisconnect::what(void) const throw()
{
    std::cerr << RED << "Throw Client disconnect" << RESET;
    return ("");
}

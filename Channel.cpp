#include "includes/Channel.hpp"

/******************************/
/***CONSTRUCTORS/DESTRUCTORS***/
/******************************/

Channel::Channel() : _channelName(""), _channelPassword(""), _topic("")
{
}

Channel::Channel(std::string channelName, std::string channelPassword) : _channelName(channelName), _channelPassword(channelPassword), _topic("")
{
}

Channel::Channel(const Channel &src) : _channelName(src._channelName), _channelPassword(src._channelPassword), _connectedClients(src._connectedClients), _connectedOperators(src._connectedOperators), _topic(src._topic)
{
}

Channel::~Channel(){};

/******************************/
/***********OPERATORS**********/
/******************************/

Channel& Channel::operator=(const Channel &rhs)
{
    if (this != &rhs)
    {
        this->_channelName = rhs._channelName;
        this->_channelPassword = rhs._channelPassword;
        this->_connectedClients = rhs._connectedClients;
        this->_connectedOperators = rhs._connectedOperators;
        this->_topic = rhs._topic;
    }
    return (*this);
}

/******************************/
/***********METHODS************/
/******************************/

std::string Channel::getChannelName(void)
{
    return (_channelName);
}

std::string Channel::getChannelPassword(void)
{
    return (_channelPassword);
}

std::string Channel::getChannelTopic(void)
{
    return (_topic);
}

std::vector<Client *>& Channel::getActiveUsersVector(void)
{
    return (this->_connectedClients);
}

std::vector<Client *>& Channel::getActiveOperatorsVector(void)
{
    return (this->_connectedOperators);
}

void Channel::addClient(Client *client)
{
   this->_connectedClients.push_back(client);
}

void Channel::quitClient(Client *client)
{
    std::vector<Client *>::iterator it;

    for (it = _connectedClients.begin(); it != _connectedClients.end(); it++)
    {
        if (client->getFd() == (*it)->getFd())
        {
            if (this->isUserOperator(client) == true)
                this->quitOperator(client);
            _connectedClients.erase(it);
            return ;
        }
    }
}

void Channel::addOperator(Client *client)
{
   this-> _connectedOperators.push_back(client);
}

void Channel::quitOperator(Client *client)
{
    std::vector<Client *>::iterator it;

    for (it = _connectedOperators.begin(); it != _connectedOperators.end(); it++)
    {
        if (client->getFd() == (*it)->getFd())
        {
            _connectedOperators.erase(it);
            return ;
        }
    }
}

bool Channel::isUserOperator(Client *client)
{
    std::vector<Client *>::iterator it;

    for (it = _connectedOperators.begin(); it != _connectedOperators.end(); it++)
    {
        if (client->getFd() == (*it)->getFd())
            return (true);
    }
    return (false);
}

void Channel::printUsers(void)
{
    std::vector<Client *>::iterator it;

    for (it = this->_connectedClients.begin(); it != this->_connectedClients.end(); it++)
    {
        std::cout << "[Channel] Nickname : " <<  (*it)->getNickname() << " | ";
        std::cout << "fd : " << (*it)->getFd();
        std::cout << std::endl;
    }
}

void Channel::printOperators(void)
{
    std::vector<Client *>::iterator it;

    for (it = this->_connectedOperators.begin(); it != this->_connectedOperators.end(); it++)
        std::cout << "Operator Nickname = " << (*it)->getNickname() << " | FD = " << (*it)->getFd() << std::endl;
}

void Channel::announceNewUser(Server &server, std::string &message)
{
    std::vector<Client *>::iterator it;
    std::vector<Client *>           &clientsVector = this->getActiveUsersVector();

    for (it = clientsVector.begin(); it != clientsVector.end(); it++)
        server.sendToClient(message, (*it)->getFd());
}

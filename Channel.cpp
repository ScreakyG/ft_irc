#include "includes/Channel.hpp"

/******************************/
/***CONSTRUCTORS/DESTRUCTORS***/
/******************************/

Channel::Channel() : _channelName(""), _channelPassword(""), _topic(""), _inviteOnly(false), _topicRestricted(true), _usersLimit(0)
{
}

Channel::Channel(std::string channelName, std::string channelPassword) : _channelName(channelName), _channelPassword(channelPassword), _topic(""), _inviteOnly(false), _topicRestricted(true), _usersLimit(0)
{
}

Channel::Channel(const Channel &src) : _channelName(src._channelName), _channelPassword(src._channelPassword), _connectedClients(src._connectedClients), _connectedOperators(src._connectedOperators), _topic(src._topic), _inviteOnly(src._inviteOnly), _topicRestricted(src._topicRestricted), _usersLimit(0), _invitedUsers(src._invitedUsers)
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
        this->_inviteOnly = rhs._inviteOnly;
        this->_topicRestricted = rhs._topicRestricted;
        this->_usersLimit = rhs._usersLimit;
        this->_invitedUsers = rhs._invitedUsers;
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

std::vector<Client *>& Channel::getInvitedUsersVector(void)
{
    return (this->_invitedUsers);
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

bool Channel::isUserOnChannel(Client *client)
{
    std::vector<Client *>::iterator it;

    for (it = _connectedClients.begin(); it != _connectedClients.end(); it++)
    {
        if (client->getFd() == (*it)->getFd())
            return (true);
    }
    return (false);
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

void Channel::setInviteMode(bool value)
{
    this->_inviteOnly = value;
}

bool Channel::isInviteOnly(void)
{
    return (_inviteOnly);
}

void Channel::setTopicRestrictions(bool value)
{
    this->_topicRestricted = value;
}

bool Channel::isTopicRestricted(void)
{
    return (_topicRestricted);
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

void Channel::notifyUsers(Server &server, std::string &message, Client *clientSender)
{
    std::vector<Client *>::iterator it;
    std::vector<Client *>           &clientsVector = this->getActiveUsersVector();

    for (it = clientsVector.begin(); it != clientsVector.end(); it++)
    {
        if ((*it)->getFd() != clientSender->getFd())
            server.sendToClient(message, (*it)->getFd());
    }
}

Client* Channel::getClientOnChannel(std::string name)
{
    std::vector<Client *>::iterator it;

    for (it = _connectedClients.begin(); it != _connectedClients.end(); it++)
    {
        if ((*it)->getNickname() == name)
            return (*it);
    }
    return (NULL);
}

void Channel::changePassword(std::string password)
{
    this->_channelPassword = password;
}

unsigned long Channel::getUsersLimit(void)
{
    return (this->_usersLimit);
}

void Channel::changeUsersLimit(unsigned long limit)
{
    this->_usersLimit = limit;
}

std::string Channel::getChannelModes(void)
{
    std::string currentModes;

    if (_topicRestricted == false && _inviteOnly == false)
        return ("");

    currentModes += "+";
    if (_topicRestricted == true)
        currentModes += "t";
    if (_inviteOnly == true)
        currentModes += "i";

    return (currentModes);
}

void Channel::setTopic(std::string topic)
{
    _topic = topic;
}

bool Channel::isClientInvited(Client *client)
{
    std::vector<Client *>::iterator it;

    for (it = _invitedUsers.begin(); it != _invitedUsers.end(); it++)
    {
        if ((*it)->getFd() == client->getFd()) // Utile si le client change de nom, son fd permet de l'identifier.
            return (true);
    }
    return (false);
}

void Channel::removeClientFromWhitelist(Client *client)
{
    std::vector<Client *>::iterator it;

    for (it = _invitedUsers.begin(); it != _invitedUsers.end(); it++)
    {
        if ((*it)->getFd() == client->getFd()) // Utile si le client change de nom, son fd permet de l'identifier.
        {
            _invitedUsers.erase(it);
            return ;
        }
    }
}

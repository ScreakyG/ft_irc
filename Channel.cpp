#include "includes/Channel.hpp"

/******************************/
/***CONSTRUCTORS/DESTRUCTORS***/
/******************************/

Channel::Channel() : _channelName(""), _channelPassword("")
{
    _topic = "Default topic.";
}

Channel::Channel(std::string channelName, std::string channelPassword) : _channelName(channelName), _channelPassword(channelPassword)
{
    _topic = "Default topic.";
}

Channel::Channel(const Channel &src) : _channelName(src._channelName), _channelPassword(src._channelPassword), _connectedClients(src._connectedClients), _topic(src._topic)
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

void Channel::addClient(Client *client)
{
    _connectedClients.push_back(client);
    //Envoyer les messages de bienvenue dans le channel.
}

void Channel::quitClient(Client *client)
{
    std::vector<Client *>::iterator it;

    for (it = _connectedClients.begin(); it != _connectedClients.end(); it++)
    {
        if (client->getFd() == (*it)->getFd())
        {
            _connectedClients.erase(it);
        }
    }
}

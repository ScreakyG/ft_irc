#include "includes/Client.hpp"

#include <iostream>

static std::string getActiveUsers(Channel &channel);
static void sendTopic(Server &server, Client *client, Channel &channel);

/******************************/
/***CONSTRUCTORS/DESTRUCTORS***/
/******************************/

Client::Client() : _clientFd(-1), _clientReadBuffer(""), _clientSendBuffer(""), _nickname(""), _oldNickname(""), _username(""), _hostname(""), _enteredServerPassword(false), _hasRegistered(false), _timeoutStart(0)
{

}

Client::Client(const Client &src) : _clientFd(src._clientFd), _clientReadBuffer(src._clientReadBuffer), _clientSendBuffer(src._clientSendBuffer), _nickname(src._nickname), _oldNickname(src._oldNickname), _username(src._username), _hostname(src._hostname), _enteredServerPassword(src._enteredServerPassword), _hasRegistered(src._hasRegistered), _timeoutStart(src._timeoutStart), _clientChannels(src._clientChannels)
{

}

Client::~Client(){};

/******************************/
/***********OPERATORS**********/
/******************************/

Client& Client::operator=(const Client &rhs)
{
    if (this != &rhs)
    {
        this->_clientFd = rhs._clientFd;
        this->_clientReadBuffer = rhs._clientReadBuffer;
        this->_clientSendBuffer = rhs._clientSendBuffer;
        this->_nickname = rhs._nickname;
        this->_oldNickname = rhs._oldNickname;
        this->_username = rhs._username;
        this->_hostname = rhs._hostname;
        this->_enteredServerPassword = rhs._enteredServerPassword;
        this->_hasRegistered = rhs._hasRegistered;
        this->_timeoutStart = rhs._timeoutStart;
        this->_clientChannels = rhs._clientChannels;
    }
    return (*this);
}

/******************************/
/***********METHODS************/
/******************************/

void Client::setFd(int fd)
{
    this->_clientFd = fd;
}

int Client::getFd(void)
{
    return (this->_clientFd);
}

void Client::setNickname(std::string nickname)
{
    this->_oldNickname = this->getNickname();
    this->_nickname = nickname;
}

std::string Client::getNickname(void)
{
    return (this->_nickname);
}

std::string Client::getOldNickname(void)
{
    return (this->_oldNickname);
}

void Client::setUsername(std::string username)
{
    this->_username = username;
}

std::string Client::getUsername(void)
{
    return (this->_username);
}

void Client::setHostname(std::string hostname)
{
    this->_hostname = hostname;
}

std::string Client::getHostname(void)
{
    return (this->_hostname);
}

void Client::setRegistered(bool value)
{
    this->_hasRegistered = value;
}

bool Client::hasRegistered(void)
{
    return (this->_hasRegistered);
}

void Client::setServerPassword(bool value)
{
    this->_enteredServerPassword = value;
}

bool Client::hasEnteredServerPassword(void)
{
    return (this->_enteredServerPassword);
}

void Client::setTimeoutStart(time_t start)
{
    this->_timeoutStart = start;
}

time_t Client::getTimeoutStart(void)
{
    return (this->_timeoutStart);
}

std::string& Client::getClientReadBuffer(void) // Retourne le _clientBuffer;
{
    return (this->_clientReadBuffer);
}
void Client::addtoClientReadBuffer(std::string &string)  // Ajoute string au _clientBuffer;
{
    this->_clientReadBuffer += string;
}

std::string& Client::getClientSendBuffer(void)
{
    return (this->_clientSendBuffer);
}

void Client::addToClientSendBuffer(std::string &string)
{
    this->_clientSendBuffer += string;
}

void Client::joinChannel(Server &server, Channel &channel, std::string channelPassword)
{
    std::string message;
    std::string activeUsers;

    if (channelPassword != channel.getChannelPassword())
    {
        message = ERR_BADCHANNELKEY(this->getNickname(), channel.getChannelName());
        server.sendToClient(message, this->getFd());
        return ;
    }

    if (channelPassword == channel.getChannelPassword())
    {
        _clientChannels.push_back(&channel);
        channel.addClient(this);

        message = ":" + this->getNickname() + "!~" + this->getUsername() + "@" + this->getHostname() + " JOIN " + channel.getChannelName() + "\r\n";
        server.sendToClient(message, this->getFd());

        sendTopic(server, this, channel);

        activeUsers = getActiveUsers(channel);
        message = RPL_NAMREPLY(this->getNickname(), channel.getChannelName(), activeUsers);
        server.sendToClient(message, this->getFd());

        message = RPL_ENDOFNAMES(this->getNickname(), channel.getChannelName());
        server.sendToClient(message, this->getFd());

        // Envoyer les NAMRPLY avec tout les noms des utilisateurs.
        //std::cout << "Client joined channel : " << channel.getChannelName() << " | password = " << channel.getChannelPassword() << std::endl;
    }
}

void Client::leaveChannel(Channel &channel)
{
    std::vector<Channel *>::iterator  it;

    for (it = _clientChannels.begin(); it != _clientChannels.end(); it++)
    {
        if (channel.getChannelName() == (*it)->getChannelName())
        {
            (*it)->quitClient(this);
            _clientChannels.erase(it);
        }
    }
}

void Client::leaveAllChannels()
{
    for (size_t idx = 0; idx < _clientChannels.size(); idx++)
        _clientChannels[idx]->quitClient(this);
}

static std::string getActiveUsers(Channel &channel)
{
    std::string activeUsers;
    std::vector<Client *>::iterator it;

    for (it = channel.getActiveUsersVector().begin(); it != channel.getActiveUsersVector().end(); it++)
    {
       // std::cout << (*it)->getNickname() << std::endl;
        activeUsers += (*it)->getNickname();
        activeUsers += " ";
    }
    return (activeUsers);
}

static void sendTopic(Server &server, Client *client, Channel &channel)
{
    std::string message;

    if (channel.getChannelTopic().empty() == true)
    {
        message = RPL_NOTOPIC(client->getNickname(), channel.getChannelName());
        server.sendToClient(message, client->getFd());
    }
    else
    {
        message = RPL_TOPIC(client->getNickname(), channel.getChannelName(), channel.getChannelTopic());
        server.sendToClient(message, client->getFd());
    }
    return ;
}

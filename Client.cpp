#include "includes/Client.hpp"

/******************************/
/***CONSTRUCTORS/DESTRUCTORS***/
/******************************/

Client::Client() : _clientFd(-1), _clientBuffer(""), _nickname(""), _username(""), _enteredServerPassword(false), _hasRegistered(false), _timeoutStart(0)
{

}

Client::Client(const Client &src) : _clientFd(src._clientFd), _clientBuffer(src._clientBuffer), _nickname(src._nickname), _username(src._username), _enteredServerPassword(src._enteredServerPassword), _hasRegistered(src._hasRegistered), _timeoutStart(src._timeoutStart)
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
        this->_clientBuffer = rhs._clientBuffer;
        this->_nickname = rhs._nickname;
        this->_username = rhs._username;
        this->_enteredServerPassword = rhs._enteredServerPassword;
        this->_hasRegistered = rhs._hasRegistered;
        this->_timeoutStart = rhs._timeoutStart;
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
    this->_nickname = nickname;
}

std::string Client::getNickname(void)
{
    return (this->_nickname);
}

void Client::setUsername(std::string username)
{
    this->_username = username;
}

std::string Client::getUsername(void)
{
    return (this->_username);
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

std::string& Client::getClientBuffer(void) // Retourne le _clientBuffer;
{
    return (this->_clientBuffer);
}
void Client::addtoClientBuffer(std::string &string)  // Ajoute string au _clientBuffer;
{
    this->_clientBuffer += string;
}

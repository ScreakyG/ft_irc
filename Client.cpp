#include "Client.hpp"

/******************************/
/***CONSTRUCTORS/DESTRUCTORS***/
/******************************/

Client::Client() : _clientFd(-1), _nickname(""), _hasRegistered(false)
{

}

Client::Client(const Client &src) : _clientFd(src._clientFd), _nickname(src._nickname), _hasRegistered(src._hasRegistered)
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
        this->_nickname = rhs._nickname;
        this->_hasRegistered = rhs._hasRegistered;
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

void Client::setRegistered(bool value)
{
    this->_hasRegistered = value;
}

bool Client::hasRegistered(void)
{
    return (this->_hasRegistered);
}

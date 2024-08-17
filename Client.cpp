#include "Client.hpp"

/******************************/
/***CONSTRUCTORS/DESTRUCTORS***/
/******************************/

Client::Client() : _clientFd(-1)
{

}

Client::Client(const Client &src) : _clientFd(src._clientFd)
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

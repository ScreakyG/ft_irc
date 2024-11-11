#pragma once

#include "Server.hpp"
#include "Client.hpp"
#include "Random.hpp"
#include "Responses.hpp"
#include "Channel.hpp"

class Server;
class Client;
class Channel;

struct ClientCompare {
    ClientCompare(Client* target) : _target(target) {}
    bool operator()(const Client* client) const {
        return client == _target;
    }
    Client* _target;
};

void exec_PASS(Server &server, std::vector<std::string> &arguments, int clientFd);
void exec_NICK(Server &server,std::vector<std::string> &arguments, int clientFd);
void exec_USER(Server &server, std::vector<std::string> &arguments, int clientFd);
void exec_PING(Server &server, std::vector<std::string> &arguments, int clientFd);
void exec_CAP(Server &server, std::vector<std::string> &arguments, int clientFd);
void exec_JOIN(Server &server, std::string &ogString, std::vector<std::string> &arguments, int clientFd);
void exec_MODE(Server &server, std::string &ogString, std::vector<std::string> &arguments, int clientFd);
void exec_KICK(Server &server, std::string &ogString, std::vector<std::string> &arguments, int clientFd);
void exec_TOPIC(Server &server, std::vector<std::string> &arguments, int clientFd);
void exec_INVITE(Server &server, std::vector<std::string> &arguments, int clientFd);
void exec_PRIVMSG(Server &server, std::vector<std::string> &arguments, int clientFd);



// JOIN PARSING FUNCTIONS

std::vector<std::pair<std::string, std::string> >  parseInput(Server &server, Client *client, std::string &input);
std::vector<std::pair<std::string, std::string> >  pairNamesPasswords(std::vector<std::string> &namesVector, std::vector<std::string> &passwordsVector);
std::vector<std::string> splitString(std::string &channels);
void splitChannelsAndPasswords(std::string &input, std::string &channels, std::string &passwords);
void   trimString(std::string &str, const char *charset);
void checkChannelsNamesValid(Server &server, Client *client, std::vector<std::pair<std::string, std::string> > &namesAndPasswords);


// MODE ACTIONS FUNCTIONS

void sendModeReply(Server &server, Client *client, Channel *channel, std::string flags, std::string flagsArguments);
std::string modifyInviteMode(Server &server, Client *client, Channel *channel, bool removeMode);
std::string modifiyTopicRestrictions(Server &server, Client *client, Channel *channel, bool removeMode);
std::string modifyOperators(Server &server, Client *client, Channel *channel, bool removeMode, std::string argumentName, std::string &successfullFlagsArgs);
std::string modifyChannelKey(Server &server, Client *client, Channel *channel, bool removeMode, std::string argumentName, std::string &successfullFlagsArgs);
std::string modifyChannelUsersLimit(Server &server, Client *client, Channel *channel, bool removeMode, std::string argumentName, std::string &successfullFlagsArgs);

#pragma once

#include "Server.hpp"
#include "Client.hpp"
#include "Random.hpp"
#include "Responses.hpp"

class Server;
class Client;

void exec_PASS(Server &server, std::vector<std::string> &arguments, int clientFd);
void exec_NICK(Server &server,std::vector<std::string> &arguments, int clientFd);
void exec_USER(Server &server, std::vector<std::string> &arguments, int clientFd);
void exec_PING(Server &server, std::vector<std::string> &arguments, int clientFd);
void exec_CAP(Server &server, std::vector<std::string> &arguments, int clientFd);
void exec_JOIN(Server &server, std::string &ogString, std::vector<std::string> &arguments, int clientFd);



// JOIN PARSING FUNCTIONS

std::vector<std::pair<std::string, std::string> >  parseInput(Server &server, Client *client, std::string &input);
std::vector<std::pair<std::string, std::string> >  pairNamesPasswords(std::vector<std::string> &namesVector, std::vector<std::string> &passwordsVector);
std::vector<std::string> splitString(std::string &channels);
void splitChannelsAndPasswords(std::string &input, std::string &channels, std::string &passwords);
void   trimString(std::string &str, const char *charset);
void checkChannelsNamesValid(Server &server, Client *client, std::vector<std::pair<std::string, std::string> > &namesAndPasswords);

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
void exec_JOIN(Server &server, std::vector<std::string> &arguments, int clientFd);

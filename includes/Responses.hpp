#pragma once

//NICK
#define ERR_NONICKNAMEGIVEN(client) ("431 " + client + " :There is no nickname\r\n")
#define ERR_ERRONEUSNICKNAME(client, nickname) ("432 " + client + " " + nickname + " :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(client, nickname) ("432 " + client + " " + nickname + " :Nickname is already in use\r\n");


//MULTIPLE COMMANDS
#define ERR_ALREADYREGISTRED(client) ("462 " + client + " " + ":You may not reregister\r\n");
#define ERR_NEEDMOREPARAMS(client, command) ("461 " + client + " " + command + " :Not enough parameters\r\n");
#define ERR_UNKNOWNCOMMAND(client, command) ("421 " + client + " " + command + " :Unknown command\r\n");



#define RPL_WELCOME(client, username) ("001 " + client + " :Welcome to the Internet Relay Network, " + username + "\r\n");

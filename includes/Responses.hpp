#pragma once

#include <iostream>

#define ERROR_MSG(msg) ("ERROR :" + msg + "\r\n");

#define ERR_NOSUCHCHANNEL(client, channelName) (":localhost 403 " + client + " " + channelName + " :No such channel\r\n");

//NICK
#define ERR_NONICKNAMEGIVEN(client) (":localhost 431 " + client + " :There is no nickname\r\n")
#define ERR_ERRONEUSNICKNAME(client, nickname) (":localhost 432 " + client + " " + nickname + " :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(client, nickname) (":localhost 433 " + client + " " + nickname + " :Nickname is already in use\r\n");

//PASS

#define ERR_PASSWDMISMATCH(client) (":localhost 464 " + client + " :Password incorrect\r\n");

//PING

#define ERR_NOSUCHSERVER(client, servername) (":localhost 402 " + client + " " + servername + " :No such server\r\n");

//MULTIPLE COMMANDS
#define ERR_ALREADYREGISTRED(client) (":localhost 462 " + client + " " + ":You may not reregister\r\n");
#define ERR_NEEDMOREPARAMS(client, command) (":localhost 461 " + client + " " + command + " :Not enough parameters\r\n");
#define ERR_UNKNOWNCOMMAND(client, command) (":localhost 421 " + client + " " + command + " :Unknown command\r\n");
#define ERR_NOTREGISTERED ("localhost 451 * :You have not registered\r\n");


// WELCOME BURST

#define RPL_WELCOME(client, nickname) (":localhost 001 " + client + " :Welcome to the Internet Relay Network, " + nickname + "\r\n");
#define RPL_YOURHOST(client) (":localhost 002 " + client + " :Your host is localhost, running version beta.1.0\r\n");
#define RPL_CREATED(client) (":localhost 003 " + client + " :This server was created Mon Aug 12 2024 at 14:32:25 UTC\r\n");

//JOIN

#define ERR_BADCHANNELKEY(client, channelName) (":localhost 475 " + client + " " + channelName + " :Cannot join channel\r\n");
#define RPL_NOTOPIC(client, channelName) (":localhost 331 " + client + " " + channelName + " :No topic is set\r\n");
#define RPL_TOPIC(client, channelName, topic) (":localhost 332 " + client + " " + channelName + " :" + topic + "\r\n");
#define RPL_NAMREPLY(client, channelName, users) (":localhost 353 " + client + " @ " + channelName + " :" + users + "\r\n");
#define RPL_ENDOFNAMES(client, channelName) (":localhost 366 " + client + " " + channelName + " :End of /NAMES list.\r\n");

#define ERR_NOSUCHNICK(source, target) (":localhost 401 " + source + " " + target + " :No such nick/channel\r\n")
#define ERR_NOTONCHANNEL(source, channel) (":localhost 442 " + source + " " + channel + " :You're not on that channel\r\n")

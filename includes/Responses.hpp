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

//MODE

#define ERR_CHANOPRIVSNEEDED(client) (":localhost 482 " + client + " :You're not channel operator\r\n");
#define ERR_INVITEONLYCHAN(client, channelName) (":localhost 473 " + client + " " + channelName + " :Cannot join channel (+i) - you must be invited\r\n");
#define ERR_UNKNOWNMODE(client, modeFlag) (":localhost 472 " + client + " " + modeFlag + " :is not a recognized mode\r\n");
#define ERR_NOSUCHNICK(client, target) (":localhost 401 " + client + " " + target + " :No such nick/channel\r\n");
#define ERR_CHANNELISFULL(client, channelName) (":localhost 471 " + client + " " + channelName + " :Cannot join channel (+l) - channel is full, try again later\r\n");
#define ERR_NOTONCHANNEL(client, channelName) (":localhost 442 " + client + " " + channelName + " :You're not on that channel\r\n");

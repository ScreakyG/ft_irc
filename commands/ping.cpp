#include "../includes/Commands.hpp"

void exec_PING(Server &server, std::vector<std::string> &arguments, int clientFd)
{
    std::string message;

    if (arguments.size() == 0)
    {
        message = "461 PING :Not enough parameters\n";
        server.sendToClient(message, clientFd);
        return ;
    }

    message = "PONG :";
    for (size_t i = 0; i < arguments.size(); i++)
    {
        message += arguments[i];
        if (i < arguments.size() - 1)
            message += " ";
    }
    message += "\n";
    server.sendToClient(message, clientFd);
}

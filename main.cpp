#include "Colors.hpp"
#include "Server.hpp"
#include "Random.hpp"

#include <stdio.h>

const char *getClientHeader(void)
{
    const char *header = "---------------------------\n---WELCOME TO THE SERVER---\n---------------------------\n";

    return (header);
}

int main(void)
{
    Server server;
    std::cout << getClientHeader() << std::endl;

    try
    {
        server.createServerSocket();
        server.listenPort();

        // Definier si on utilise select(), poll(), epoll() ou kqueue();
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}

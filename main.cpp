#include "Colors.hpp"
#include "Server.hpp"
#include "Random.hpp"

int main(void)
{
    Server server;
    try
    {
        server.createServerSocket();
        server.listenPort();
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}

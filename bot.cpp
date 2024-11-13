#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <cerrno>
#include <unistd.h>

#define LINK        "https://www.youtube.com/watch?v=dQw4w9WgXcQ"
#define BUFFER_SIZE 512

void sendData(int socket, const std::string& data);
void handleConnection(int socket, const std::string& password, const std::string& nickname, const std::string& channel);

void sendData(int socket, const std::string& data)
{
    std::string msg = data + "\r\n";
    if (send(socket, msg.c_str(), msg.length(), 0) == -1)
        std::cerr << "Error sending: " << data << std::endl;
}

void handleConnection(int socket, const std::string& password, const std::string& nickname, const std::string& channel)
{
    sendData(socket, "PASS " + password);
    sendData(socket, "NICK " + nickname);
    sendData(socket, "USER " + nickname + " 0 * :" + nickname);
    sleep(1);
    sendData(socket, "JOIN " + channel);
}

int main(int ac, char** av)
{
    if (ac != 5)
    {
        std::cout << "Usage: " << av[0] << " <port> <password> <channel> <nickname>" << std::endl;
        return 1;
    }

    std::string server = "127.0.0.1";
    std::string port = av[1];
    std::string password = av[2];
    std::string channel = av[3];
    std::string nickname = av[4];

    if (channel[0] != '#')
        channel = "#" + channel;

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(server.c_str(), port.c_str(), &hints, &res) != 0)
    {
        std::cerr << "Failed to get address info" << std::endl;
        return 1;
    }

    int socketDescriptor = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (socketDescriptor == -1)
    {
        std::cerr << "Failed to create socket" << std::endl;
        freeaddrinfo(res);
        return 1;
    }

    if (connect(socketDescriptor, res->ai_addr, res->ai_addrlen) == -1)
    {
        std::cerr << "Failed to connect to server" << std::endl;
        close(socketDescriptor);
        freeaddrinfo(res);
        return 1;
    }

    freeaddrinfo(res);
    handleConnection(socketDescriptor, password, nickname, channel);

    char buffer[BUFFER_SIZE];
    while (true)
    {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(socketDescriptor, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytesReceived <= 0)
        {
            std::cerr << "Connection closed" << std::endl;
            break;
        }

        std::string receivedData(buffer);
        std::cout << receivedData;

        if (receivedData.find("PING :") != std::string::npos)
        {
            sendData(socketDescriptor, "PONG :" + receivedData.substr(receivedData.find("PING :") + 6));
            continue;
        }

        if (receivedData.find("PRIVMSG " + channel) != std::string::npos)
        {
            size_t bangPos = receivedData.find('!');
            if (bangPos != std::string::npos)
            {
                std::string sender = receivedData.substr(1, bangPos - 1);
                std::string messageContent;
                size_t msgPos = receivedData.find(" :");
                
                if (msgPos != std::string::npos)
                {
                    messageContent = receivedData.substr(msgPos + 2);
                    // Remove \r\n
                    while (!messageContent.empty() && (messageContent[messageContent.length()-1] == '\r' || 
                           messageContent[messageContent.length()-1] == '\n'))
                    {
                        messageContent.erase(messageContent.length()-1);
                    }

                    if (messageContent == "!hello")
                    {
                        sleep(1);
                        sendData(socketDescriptor, "PRIVMSG " + channel + " :Hello, " + sender + "!");
                    }
                    else if (messageContent == "!link")
                    {
                        sleep(1);
                        sendData(socketDescriptor, "PRIVMSG " + channel + " :" + LINK);
                    }
                    else if (messageContent == "!creator")
                    {
                        sleep(1);
                        sendData(socketDescriptor, "PRIVMSG " + channel + " :fgonzale et fbesson!");
                    }
                }
            }
        }
    }
    close(socketDescriptor);
    return 0;
}

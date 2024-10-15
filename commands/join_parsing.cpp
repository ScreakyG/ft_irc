#include "../includes/Commands.hpp"

void   trimString(std::string &str, const char *charset)
{
    if (str.empty() == true)
        return ;

    str.erase(0, str.find_first_not_of(charset));
    str.erase(str.find_last_not_of(charset) + 1);
}

void splitChannelsAndPasswords(std::string &input, std::string &channels, std::string &passwords)
{
    size_t      channelEnd;

    channelEnd = input.find_first_of(" ");
    channels = input.substr(0, channelEnd); // Recupere la string des channels.

    if (channelEnd != std::string::npos)
        passwords = input.substr(channelEnd + 1, input.size() - (channelEnd + 1)); // Il y a des choses derriere l'espasces , c'est la string des passwords.

    trimString(channels, " ");
    trimString(passwords, " ");
}

std::vector<std::string> splitString(std::string &channels)
{
    std::vector<std::string>    namesVectors;
    std::string                 channelName;
    size_t                      begin;
    size_t                      end;

    begin = 0;
    end = channels.find_first_of(",");

    if (end == std::string::npos) // Il n'y a que un seul channel.
    {
        channelName = channels.substr(begin, end - begin);
        namesVectors.push_back(channelName);

        return (namesVectors);
    }

    // Pour le cas ou il y a plusieurs channels.
    while (end != std::string::npos)
    {
        channelName = channels.substr(begin, end - begin);
        namesVectors.push_back(channelName);

        begin = end + 1;
        end = channels.find_first_of(',', end + 1);

        if (end == std::string::npos) // Pour le denier channel, end ne trouveras pas de "," et renverra npos.
        {
            channelName = channels.substr(begin, end - begin);
            namesVectors.push_back(channelName);
            break ;
        }
    }
    return (namesVectors);
}


std::vector<std::pair<std::string, std::string> >  pairNamesPasswords(std::vector<std::string> &namesVector, std::vector<std::string> &passwordsVector)
{
    std::vector<std::pair<std::string, std::string> >   vector;
    std::pair<std::string, std::string>  pair;

    for (size_t idx = 0; idx < namesVector.size(); idx++)
    {
        pair.first = namesVector[idx];
        if (idx < passwordsVector.size() && passwordsVector[idx].empty() == false)
            pair.second = passwordsVector[idx];
        else
            pair.second = "NO_PASSWORD";
        vector.push_back(pair);
    }
    return (vector);
}

std::vector<std::pair<std::string, std::string> >  parseInput(Server &server, Client *client, std::string &input)
{
    std::vector<std::pair<std::string, std::string> >   vector;
    std::vector<std::string>                            namesVector;
    std::vector<std::string>                            passwordsVector;
    std::string                                         channels;
    std::string                                         passwords;
    std::string::iterator                               it;

    if (input.empty())
        return (vector);

    trimString(input, " ");
    //std::cout << input << std::endl;

    splitChannelsAndPasswords(input, channels, passwords);
    if (channels.empty())
        return (vector);

    namesVector = splitString(channels);
    passwordsVector = splitString(passwords);
    vector = pairNamesPasswords(namesVector,passwordsVector);

    checkChannelsNamesValid(server, client, vector);

    return (vector);
}

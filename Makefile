NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -MMD -g -std=c++98

SRCS = main.cpp Server.cpp Channel.cpp Client.cpp commands/pass.cpp commands/nick.cpp commands/user.cpp commands/ping.cpp commands/join.cpp commands/join_parsing.cpp commands/cap.cpp commands/mode.cpp commands/mode_actions.cpp commands/kick.cpp commands/topic.cpp commands/invite.cpp commands/privmsg.cpp commands/part.cpp commands/quit.cpp

SRCS_OBJS = $(SRCS:.cpp=.o)

DEPS = $(SRCS:.cpp=.d)

all: $(NAME)

$(NAME): $(SRCS_OBJS)
		$(CXX) $(CXXFLAGS) $(SRCS_OBJS) -o $(NAME)

clean:
		rm -rf $(SRCS_OBJS) $(DEPS) bot.d bot.o

fclean: clean
		rm -rf $(NAME)
		rm -rf bot

re: fclean all

bot:
	c++ $(CXXFLAGS) bot.cpp -o bot

-include $(DEPS)

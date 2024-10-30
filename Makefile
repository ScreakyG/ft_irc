NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -MMD -g -std=c++98

SRCS = main.cpp Server.cpp Channel.cpp Client.cpp commands/pass.cpp commands/nick.cpp commands/user.cpp commands/ping.cpp commands/join.cpp commands/join_parsing.cpp commands/cap.cpp commands/privmsg.cpp
SRCS_OBJS = $(SRCS:.cpp=.o)

DEPS = $(SRCS:.cpp=.d)

all: $(NAME)

$(NAME): $(SRCS_OBJS)
		$(CXX) $(CXXFLAGS) $(SRCS_OBJS) -o $(NAME)

clean:
		rm -rf $(SRCS_OBJS) $(DEPS)

fclean: clean
		rm -rf $(NAME)

re: fclean all
 
-include $(DEPS)

CC = c++
CFLAGS = -std=c++98 -Wall -Wextra -Werror -g3

NAME = Kserver

SRCS = main.cpp	\
			 Kserver.cpp \
			 Kqueue.cpp

CSRCS = echo_client.cpp

RM = rm -rf

OBJS = $(SRCS:.cpp=.o)

COBJS = $(CSRCS:.cpp=.o)

all : Server client
Server : $(NAME)

$(NAME) : $(OBJS)
		$(CC) $(CFLAGS) -o $@ $^

%.o : %.cpp
		$(CC) $(CFLAGS) -o $@ -c $^

client : $(COBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
		$(RM) $(OBJS)
		$(RM) $(COBJS)

fclean: clean
		$(RM) $(NAME)
		$(RM) client

re:
	make fclean
	make all

.PHONY: all clean fclean re

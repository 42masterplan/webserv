CC = c++
CFLAGS = -std=c++98 -pedantic  -Wall -Wextra -Werror -g3 -fsanitize=address

NAME = Passer

SRCS = main.cpp	\
			 ConfParser.cpp \
			 HttpBlock.cpp \
			 LocBlock.cpp \
			 ServBlock.cpp \
			 ParseTool.cpp \
			 OtherBlock.cpp \
			 HttpBase.cpp


RM = rm -rf

OBJS = $(SRCS:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJS)
		$(CC) $(CFLAGS) -o $@ $^

%.o : %.cpp
		$(CC) $(CFLAGS) -o $@ -c $^


clean:
		$(RM) $(OBJS)

fclean: clean
		$(RM) $(NAME)

re:
	make fclean
	make all

.PHONY: all clean fclean re

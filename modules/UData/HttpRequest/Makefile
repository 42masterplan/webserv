NAME = webserv
CXX = c++
CXXFLAGS = -std=c++98 -pedantic -Wall -Wextra -Werror -g3 -fsanitize=address

#main문
MAINSRC = main.cpp HttpRequest2.cpp
MAINOBJ = $(addprefix ./, $(MAINSRC:.cpp=.o))

#클라이언트
# CSRCS = echo_client.cpp
# COBJS = $(CSRCS:.cpp=.o)

OBJS  =  $(MAINOBJ) 

RM = rm -rf

all : $(NAME)

$(NAME) : $(OBJS)
		$(CXX) $(CXXFLAGS) -o $@ $^

%.o : %.cpp
		$(CXX) $(CXXFLAGS) -o $@ -c $^

client : $(COBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

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

NAME = webserv
CXX = c++
CXXFLAGS = -std=c++98 -pedantic -Wall -Wextra -Werror -g3 -fsanitize=address

#main문
MAINSRC = main.cpp HttpRequest.cpp ../../Tools/ParseTool.cpp
MAINOBJ = $(addprefix ./, $(MAINSRC:.cpp=.o))
CONFIGSRCS = ConfParser.cpp \
						httpBlock/HttpBlock.cpp \
						httpBlock/HttpBase.cpp \
						LocationBlock/LocBlock.cpp \
						serverBlock/ServBlock.cpp \
						otherBlock/OtherBlock.cpp
CONFIGOBJS = $(addprefix ../../config/, $(CONFIGSRCS:.cpp=.o))
#클라이언트
# CSRCS = echo_client.cpp
# COBJS = $(CSRCS:.cpp=.o)

OBJS  =  $(MAINOBJ) $(CONFIGOBJS)

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

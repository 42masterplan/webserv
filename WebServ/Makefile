NAME = webserv
CXX = c++
CXXFLAGS = -std=c++98 -pedantic  -Wall -Wextra -Werror -g3 -fsanitize=address

#conf_file
CONFIGSRCS = ConfParser.cpp \
						httpBlock/HttpBlock.cpp \
						httpBlock/HttpBase.cpp \
						LocationBlock/LocBlock.cpp \
						serverBlock/ServBlock.cpp \
						Tools/ParseTool.cpp \
						otherBlock/OtherBlock.cpp

CONFIGOBJS = $(addprefix ./modules/config/, $(CONFIGSRCS:.cpp=.o))

#WebServ
WEBSERVSRCS = Kqueue.cpp \
							WebServ.cpp \
							ServManager.cpp \
							UData.cpp

WEBSERVOBJS = $(addprefix ./modules/webserv/, $(WEBSERVSRCS:.cpp=.o))

#main문
MAINSRC = main.cpp
MAINOBJ = $(addprefix ./, $(MAINSRC:.cpp=.o))

#클라이언트
CSRCS = echo_client.cpp
COBJS = $(CSRCS:.cpp=.o)


OBJS  =  $(CONFIGOBJS) $(WEBSERVOBJS) $(MAINOBJ)

RM = rm -rf

all : $(NAME) client

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

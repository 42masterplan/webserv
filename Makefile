NAME = webserv
CXX = c++
CXXFLAGS = -std=c++98 -pedantic -Wall -Wextra -Werror -g3 -fsanitize=address

#conf_file
CONFIGSRCS = ConfParser.cpp \
						httpBlock/HttpBlock.cpp \
						httpBlock/HttpBase.cpp \
						LocationBlock/LocBlock.cpp \
						serverBlock/ServBlock.cpp \
						otherBlock/OtherBlock.cpp

CONFIGOBJS = $(addprefix ./modules/config/, $(CONFIGSRCS:.cpp=.o))

#WebServ
WEBSERVSRCS = Kqueue.cpp \
							WebServ.cpp \
							ServManager.cpp \
							AutoIndex.cpp  \
							Cgi.cpp


WEBSERVOBJS = $(addprefix ./modules/webserv/, $(WEBSERVSRCS:.cpp=.o))

#UData
UDATASRCS = UData.cpp \
						HttpRequest/HttpRequest.cpp \
						HttpResponse/HttpResponse.cpp \
						HttpResponse/MimeStore.cpp \
						HttpResponse/StatusMsgStore.cpp \
						HttpResponse/HttpResponseHandler.cpp
#HttpResponse/HttpResponse.cpp
UDATASOBJS = $(addprefix ./modules/UData/, $(UDATASRCS:.cpp=.o))

TOOLSRCS = ParseTool.cpp 
TOOLOBJS = $(addprefix ./modules/Tools/, $(TOOLSRCS:.cpp=.o))

#main문
MAINSRC = main.cpp
MAINOBJ = $(addprefix ./, $(MAINSRC:.cpp=.o))

#클라이언트
CSRCS = echo_client.cpp
COBJS = $(CSRCS:.cpp=.o)

OBJS  =  $(CONFIGOBJS) $(WEBSERVOBJS) $(UDATASOBJS) $(TOOLOBJS) $(MAINOBJ) 

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

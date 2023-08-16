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

EVENTSRCS = 	Features/Cgi.cpp \
							Features/Kqueue.cpp \
							Handler/EventHandler.cpp \
							Handler/HttpResponseHandler.cpp 

EVENTOBJS = $(addprefix ./modules/Event/, $(EVENTSRCS:.cpp=.o))

#WebServ
WEBSERVSRCS = WebServ.cpp \
							ServManager.cpp 

WEBSERVOBJS = $(addprefix ./modules/webserv/, $(WEBSERVSRCS:.cpp=.o))

#UData
UDATASRCS = UData.cpp \
						HttpRequest/HttpRequest.cpp \
						HttpResponse/HttpResponse.cpp \
						HttpResponse/Store/MimeStore.cpp \
						HttpResponse/Store/StatusMsgStore.cpp \
						HttpResponse/Features/AutoIndex.cpp \
						HttpResponse/Features/Session.cpp

#HttpResponse/HttpResponse.cpp
UDATASOBJS = $(addprefix ./modules/UData/, $(UDATASRCS:.cpp=.o))

TOOLSRCS = ParseTool.cpp FileTool.cpp
TOOLOBJS = $(addprefix ./modules/Tools/, $(TOOLSRCS:.cpp=.o))

#main문
MAINSRC = main.cpp
MAINOBJ = $(addprefix ./, $(MAINSRC:.cpp=.o))

TEST_FILE = ./var/www/YoupiBanane/upload_put/file_should_exist_after.txt

OBJS  =  $(CONFIGOBJS) $(EVENTOBJS) $(WEBSERVOBJS) $(UDATASOBJS) $(TOOLOBJS) $(MAINOBJ) 

RM = rm -rf

all : $(NAME) 

$(NAME) : $(OBJS)
		$(CXX) $(CXXFLAGS) -o $@ $^

%.o : %.cpp
		$(CXX) $(CXXFLAGS) -o $@ -c $^


clean:
		$(RM) $(OBJS)
		$(RM) $(COBJS)

fclean: clean
		$(RM) $(NAME)
		$(RM) $(TEST_FILE)
re:
	make fclean
	make all

.PHONY: all clean fclean re

NAME = test_conf
CXX = c++
CXXFLAGS = -std=c++98 -pedantic -Wall -Wextra -Werror -g3 -fsanitize=address


CONFIGSRCS = ConfParser.cpp \
						httpBlock/HttpBlock.cpp \
						httpBlock/HttpBase.cpp \
						LocationBlock/LocBlock.cpp \
						serverBlock/ServBlock.cpp \
						otherBlock/OtherBlock.cpp \
						main.cpp
CONFIGOBJS = $(addprefix ./, $(CONFIGSRCS:.cpp=.o))
TOOLSRCS = ParseTool.cpp 
TOOLOBJS = $(addprefix ../Tools/, $(TOOLSRCS:.cpp=.o))

RM = rm -rf

all : $(NAME) 

$(NAME) : $(CONFIGOBJS) $(TOOLOBJS)
		$(CXX) $(CXXFLAGS) -o $@ $^

%.o : %.cpp
		$(CXX) $(CXXFLAGS) -o $@ -c $^


clean:
		$(RM) $(CONFIGOBJS)
		$(RM) $(TOOLOBJS)

fclean: clean
		$(RM) $(NAME)

re:
	make fclean
	make all

.PHONY: all clean fclean re

# include "ConfParser.hpp"

int main(int argc, char** argv){
	ConfParser::getInstance().configParseAll(argc, argv);
	ServBlock s= ConfParser::getInstance().getServBlock(80,"localhost");
	// int fd = open("./serverBlock/",O_RDONLY);
	// std::cout <<"fd에용" <<fd;
	// s.findLocBlock("/directory").printInfo();
	// s.findLocBlock("/directory/youpi.bad_extension").printInfo();
	// s.findLocBlock("/directory/nop").printInfo();
	// s.findLocBlock("/directory/nop/").printInfo();
	s.findLocBlock("/directory/CGI_2.py").printInfo();
	// s.findLocBlock("/directory/nop/other.pouic").printInfo();
}

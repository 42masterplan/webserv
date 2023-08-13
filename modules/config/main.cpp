# include "ConfParser.hpp"

int main(int argc, char** argv){
	ConfParser::getInstance().configParseAll(argc, argv);
	ServBlock s= ConfParser::getInstance().getServBlock(80,"localhost");
	s.findLocBlock("/").printInfo();
}
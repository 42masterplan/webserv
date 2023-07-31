#include "ConfParser.hpp"


int main(int argc, char **argv){
	try{
		ConfParser::getInstance().configParseAll(argc, argv);
		ServBlock tmp = ConfParser::getInstance().getServBlock(8080, "localhost");
		tmp.printInfo();
		std::cout << "--------------------------loccccccc-----------------------------\n";
		tmp.findLocBlock("/.pyth").printInfo();
	}catch (std::exception &e){
		std::cerr << e.what() << "\n";
	}
}

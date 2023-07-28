#include "ConfParser.hpp"


int main(int argc, char **argv){
	try{
		if (argc > 2)
			throw(std::runtime_error("You can only one conffile or default file"));
		ConfParser::getInstance().confInit();
		if (argc == 2)
			ConfParser::getInstance().setConfPath(argv[1]);//this is my conf_file
		ConfParser::getInstance().refineDirective();
		ServBlock tmp = ConfParser::getInstance().getServBlock(8080, "localhost");
		tmp.printInfo();
		std::cout << "--------------------------loccccccc-----------------------------\n";
		tmp.findLocBlock("/.pyth").printInfo();
	}catch (std::exception &e){
		std::cerr << e.what() << "\n";
	}
}

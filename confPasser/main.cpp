#include "ConfParser.hpp"


int main(int argc, char **argv){
	try{

		if (argc > 2)
			throw(std::runtime_error("You can only one conffile or default file"));
		ConfParser test; //this is default confile
			if (argc == 2)
				test.setConfPath(argv[1]);//this is my confile
		test.confInit();
		test.refineDirective();
		ServBlock tmp = test.getServBlock(8080, "localhost");
		tmp.printInfo();
		std::cout << "--------------------------loccccccc-----------------------------\n";
		tmp.findLocBlock("/.pyth").printInfo();

	}catch (std::exception &e){
		std::cerr << e.what() << "\n";
	}
}

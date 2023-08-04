# include "WebServ.hpp"

void	WebServ::startServer(int argc, char** argv){
	try{
		ConfParser::getInstance().configParseAll(argc, argv);
		// ServBlock s= ConfParser::getInstance().getServBlock(8080,"localhost");
		// s.findLocBlock("index.shsh").printInfo();
		ServManager::getInstance().serverInit();
		ServManager::getInstance().launchServer();
	}catch(std::exception &e){
		std::cerr << e.what();
	}
}


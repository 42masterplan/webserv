# include "WebServ.hpp"

void	WebServ::startServer(int argc, char** argv){
	try{
		ConfParser::getInstance().configParseAll(argc, argv);
		ServManager::getInstance().serverInit();
		ServManager::getInstance().launchServer();
	}catch(std::exception &e){
		std::cerr << e.what();
	}
}


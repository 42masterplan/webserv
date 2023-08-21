# include "WebServ.hpp"

void	WebServ::startServer(int argc, char** argv){
	try{
		ConfParser::getInstance().configParseAll(argc, argv);
		ServManager::getInstance().serverInit();
		ServManager::getInstance().launchServer();
	}catch(std::exception &e){//config에서만 에러가 날 수 있습니다.
		std::cerr << e.what();
	}
}


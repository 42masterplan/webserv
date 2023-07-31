#include "Kserver.hpp"

void leaks(){
	system("leaks Kserver");
}

int main(int argc, char **argv){
	atexit(leaks);
	try{
		if (argc != 2)
			throw(std::invalid_argument("Only one ARGV for <port>"));
		Kserver serv(argv[1]);
		serv.serverInit();
		serv.launchServer();
	}
	catch(std::exception &e){
		std::cerr << e.what() << "\n";
	}
}

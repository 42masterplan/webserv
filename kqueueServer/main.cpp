#include "Kserver.hpp"

int main(int argc, char **argv){
	try{
		if (argc != 2)
			throw(std::invalid_argument("Only one ARGV for <port>"));
		Kserver serv(argv[1]);
		serv.serverInit();
		serv.startWorking();
	}
	catch(std::exception &e){
		std::cerr << e.what() << "\n";
	}
}

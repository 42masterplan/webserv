# include "./modules/webserv/WebServ.hpp"

int main(int argc, char **argv){
	try{
		WebServ WebServ;
		WebServ.startServer(argc,argv);
	}
	catch(std::exception &e){//config parsing에서만 올 수 있다.
		std::cerr << e.what() << "\n";
	}
}

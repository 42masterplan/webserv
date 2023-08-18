# include "./modules/webserv/WebServ.hpp"

int main(int argc, char **argv){
	try{
		WebServ WebServ;
		WebServ.startServer(argc,argv);
	}
	catch(std::exception &e){
		std::cerr << e.what() << "\n";
	}
}

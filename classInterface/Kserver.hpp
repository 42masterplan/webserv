#ifndef KSERVER_HPP
# define KSERVER_HPP
# include <sys/event.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <fcntl.h>
# include <iostream>
# include <cstdio>
# include "Kqueue.hpp"
# include "ConfParser.hpp"
# include "Client.hpp"
class Kserver{
	public :
		static const int BUFF_SIZE = 500;

		explicit Kserver(char *port);
		~Kserver();
		void  serverInit(); //TODO: conf parser init();
		// void  sockAccept();//현재 사용하지 않습니다.
		void  startWorking();

	private:
		Kserver();
		void  sockInit();
		void  sockBind();
		void  sockListen();
		
		void  handleEvents();
		void  registerNewClnt();
		void  handleReadables(struct kevent *cur_event);
		void  sockWriteable(struct kevent *cur_event);
		void  disconnectClient(int clnt_fd);
		ConfParser conf_parser_;
		Kqueue kqueue_;
		struct kevent event_list_[8];//한번 감지된 이벤트의 배열
		int event_list_size_;
		char buff_[BUFF_SIZE];
		int port_;

		//server socket
		int serv_sockfd_;
		struct sockaddr_in serv_addr_;
		socklen_t serv_addrsz_;

		//client socket
		int clnt_sockfd_;
		struct sockaddr_in clnt_addr_;
		socklen_t clnt_addrsz_;
		//클라언트의 소켓 번호와 보내는 데이터를 담는 변수

		std::map<int, Client> clnt_store_;//미정
		// std::vector<Client> clnt_store_;
		
};


#endif
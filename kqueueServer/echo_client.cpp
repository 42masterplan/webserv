#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <exception>
//클라이언트
//1. 피연산자 갯수 전달
//2. 피연산자 1개씩 입력 (따로 숫자가 아닌 값이 들어오는 예외는 처리하지 않음.)
//3. operator 전달.
//4. 전달이 끝나고 결과값을 받고 연결을 종료.
// #define BUFF_SIZE 500

class Client{
	public :
		static const int BUFF_SIZE = 500;

		void  initServAdr(char *ip_ad, char* port);
		void  makeSock();
		const int& getSock();
		void  ConnectWithServer();
		void  sendData();
		void  receiveResult();
		void  closeConnect();
	private :
		int sockfd;
		struct sockaddr_in serv_adr;
		char buff[BUFF_SIZE];
};

void Client:: initServAdr(char *ip_ad, char* port){
	std::memset(&serv_adr, 0,sizeof(serv_adr));
	inet_aton(ip_ad, &serv_adr.sin_addr);
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_port = htons(std::atoi(port));
}

void Client:: makeSock(){
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
}

const int &Client:: getSock(){
	return (sockfd);
}

void  Client::ConnectWithServer(){
	if (connect(sockfd, (sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
		throw(std::runtime_error("connect error"));
	std::cout << "Connected:..............\n";
}

void  Client::sendData(){
	std::cout << "SEND TO MESSAGE TO SERVER: ";
	std::cin >> buff;
	write(sockfd, &buff, BUFF_SIZE);
	// fflush(sockfd);
}

void  Client::receiveResult(){
	int len = read(sockfd, buff, BUFF_SIZE);
	if (len == 0)
		return ;
	else{
		buff[len ] = '\0';
		std::cout <<"FROM SERVER : " <<buff << "\n";
	}
}

void Client::closeConnect(){
	close(sockfd);
}

int main(int argc, char *argv[]){
	if (argc != 3){
		std::cerr << "error : <ip> <port>\n";
		return (1);
	}
	try{
		Client s1;
		s1.initServAdr(argv[1], argv[2]);
		s1.makeSock();
		s1.ConnectWithServer();
		while (1){
			s1.sendData();
			s1.receiveResult();
		}
		s1.closeConnect();
	}
	catch(std::exception &e){
		std::cout << e.what() << "\n";
	}
}

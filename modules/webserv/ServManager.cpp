#include "ServManager.hpp"

ServManager::ServManager(){}

/**
 * @brief 싱글톤 패턴을 위해서 static 변수를 반환합니다.
 * @return ServManager& 싱글톤 패턴을 위해서 static 변수를 반환합니다.
 */
ServManager& ServManager::getInstance(){
	static ServManager serv_manager;
	return serv_manager;
}


/**
 * @brief ServManager 소멸자입니다. 서버 소켓을 닫습니다.
 */
ServManager::~ServManager() {
	for (size_t i = 0; i < serv_sock_fds_.size(); i++){
		close(serv_sock_fds_[i]);
	}
}


/**
 * @brief Listen할 포트를 가져와 Server소켓을 생성 후 초기화합니다. init, bind, listen
 * 
 */
void  ServManager::serverInit(){
	listen_ports_ = ConfParser::getInstance().getListenPorts();
	signal(SIGPIPE, SIG_IGN);
	for (size_t i = 0; i < listen_ports_.size(); i++){
		sockInit();
		sockBind(listen_ports_[i]);
		sockListen();
		std::cout << "listen PORT: " << listen_ports_[i] << std::endl;
	}
}


/**
 * @brief 메인로직을 구성하는 함수로, kqueue를 시작하고 무한루프를 돌면서 이벤트를 감지->처리합니다.
 */
void  ServManager::launchServer(){
	Kqueue::kqueueStart(serv_sock_fds_);
	while (1){
		event_list_size_ = Kqueue::detectEvent(event_list_);
		handleEvents();
	}
}


/**
 * @brief 서버소켓을 초기화합니다. SO_REUSEADDR로 포트를 재사용하게 해주었습니다.
 * @exception 포트번호가 적절치 않거나 소켓 생성에 실패할 경우 throw 합니다.
 */
void  ServManager::sockInit(){
	int serv_sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sockfd == -1)
		throw(std::runtime_error("SOCK() ERROR"));
	int	optval = 1;
  struct linger _linger;

  _linger.l_onoff = 1;//링거 옵션 활상화
  _linger.l_linger = 0; //close가 즉시 리턴을 해서 상태가 종료 되고 소켓 버퍼에 남아있는 데이터를 버리는 비정상 종료가 이루어집니다.
	if (setsockopt(serv_sockfd, SOL_SOCKET, SO_LINGER, &_linger, sizeof(_linger)))  
		throw(std::runtime_error("Socket opt Change ERROR!!(SO_LINGER)"));
	//keep alive 설정해줌
	//SO_KEEPALIVE 옵션은 TCP 소켓에 적용된다. 이 옵션을 1로 설정하면 일정 시간(통상 2시간)동안 해당 소켓을 통해 어떤 자료도 송수신되지 않을 때, 커널에서 상대방의 상태를 확인하는 패킷을 전송한다. 이 패킷에 대해 상대방이 정상적이면 ACK 패킷을 전송한다.
	// ACK 패킷으로 정상이라고 응답하는 경우 응용 프로그램에는 어떠한 통보도 하지 않고 커널 간의 확인만으로 상대방이 살아 있음을 확인하고 마무리한다. 상대방으로부터 아무런 응답이 없거나 RST 응답을 받으면 소켓을 자동으로 종료한다.
	if (setsockopt(serv_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) == -1)
		throw(std::runtime_error("Socket opt Change ERROR!!(SO_KEEPALIVE)"));
	optval = 1;
	//TCP레벨의 변경사항 -> Nagle 알고리즘 끄기(지연전송 해제) -> 전송속도 up && network부하 up
	if (setsockopt(serv_sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval)) == -1) 
		throw(std::runtime_error("Socket opt Change ERROR!!(TCP_NODELAY)"));
	
	if (setsockopt(serv_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		throw(std::runtime_error("Socket opt Change ERROR!!(SO_REUSEADDR)"));
	serv_sock_fds_.push_back(serv_sockfd);
}


/**
 * @brief 서버의 serv_addr_ 변수를 초기화하고 bind()를 호출합니다.
 * @exception bind()에러 발생 시 runtime_error를 throw합니다.
 */
void  ServManager::sockBind(int port){
	struct sockaddr_in	serv_addr;

	std::memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);
	serv_addr.sin_family = AF_INET;
	if (bind(serv_sock_fds_[serv_sock_fds_.size() - 1], (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
		throw(std::runtime_error("BIND() ERROR"));
}


/**
 * @brief listen()함수에 서버소켓과 1024칸의 연결대기큐 사이즈 인자를 주어 호출합니다.
 * @note 여기서 서버 fd를 non-blocking으로 바꿔줍니다.
 * @exception listen()에서 에러 발생 시 runtime_error를 throw합니다.
 */
void  ServManager::sockListen(){
	if (listen(serv_sock_fds_[serv_sock_fds_.size() - 1], 1024) == -1)
		throw(std::runtime_error("LISTEN() ERROR"));
	fcntl(serv_sock_fds_[serv_sock_fds_.size() - 1], F_SETFL, O_NONBLOCK);
}

/**
 * @brief 코어함수로, kqueue에서 받아온 이벤트들을 하나씩 처리합니다.
 * @exception read, write이외의 이벤트가 발생했을 시 runtime_error를 throw합니다.
 */
void  ServManager::handleEvents(){
	struct kevent*	event;
  UData*					udata;
	e_fd_type				fd_type;

	for (int i = 0; i < event_list_size_; i++){
		event = &event_list_[i];
		if (event->udata != NULL){
			udata = (UData*)event->udata;
			fd_type = udata->fd_type_;
		}
		if (std::find(serv_sock_fds_.begin(), serv_sock_fds_.end(),event->ident) != serv_sock_fds_.end())
			registerNewClnt(event->ident);
		else if (event->udata == NULL || event->flags & EV_ERROR)
			continue;
		else if (fd_type == CLNT && event->filter == EVFILT_READ)
			EventHandler::getInstance().sockReadable(event);
		else if (fd_type == CLNT && event->filter == EVFILT_WRITE)
			EventHandler::getInstance().sockWritable(event);
		else if (fd_type == CGI && event->filter == EVFILT_READ)
			EventHandler::getInstance().cgiReadable(event);
    else if (fd_type == CGI && event->filter == EVFILT_WRITE)
      EventHandler::getInstance().cgiWritable(event);
    else if (fd_type == CGI && event->filter == EVFILT_TIMER)
      EventHandler::getInstance().cgiTimeout(event);
		else if (fd_type == FILETYPE && event->filter == EVFILT_READ)
			EventHandler::getInstance().fileReadable(event);
		else if (fd_type == FILETYPE && event->filter == EVFILT_WRITE)
			EventHandler::getInstance().fileWritable(event);
		else{
			EventHandler::getInstance().disconnectFd(event);
			continue;
    }
	}
}

/**
 * @brief 새로운 클라이언트를 등록하는 함수입니다.
 * 클라이언트 소켓을 accept 해주고 non-blocking으로 만든 후, kqueue 이벤트에 read / write 모두 등록합니다.
 * 클라이언트 UData를 생성해 인자로 등록합니다.
 * @exception accept()에서 에러 발생 시 runtime_error를 throw합니다.
 */
void  ServManager::registerNewClnt(int serv_sockfd){
	size_t idx;
	struct sockaddr_in	clnt_addr;
	socklen_t						clnt_addrsz = sizeof(clnt_addr);
	int									clnt_sockfd = accept(serv_sockfd, (struct sockaddr *) &clnt_addr, &clnt_addrsz);

	if (clnt_sockfd == -1)
		throw(std::runtime_error("ACCEPT() ERROR"));
	fcntl(clnt_sockfd, F_SETFL, O_NONBLOCK);
	struct linger _linger;
  _linger.l_onoff = 1;
  _linger.l_linger = 0;
  setsockopt(clnt_sockfd, SOL_SOCKET, SO_LINGER, &_linger, sizeof(_linger));  
	for (idx = 0; idx < serv_sock_fds_.size(); idx++){
		if (serv_sock_fds_[idx] == serv_sockfd)
			break;
	}
	UData*	udata_ptr = new UData(CLNT, listen_ports_[idx]);
	std::cout << GREEN << "Connected Client: " << clnt_sockfd << CLOSE << std::endl;
	std::cout << GREEN << "my_port:::" << listen_ports_[idx] << CLOSE << std::endl;
	udata_ptr->client_fd_ = clnt_sockfd;
	Kqueue::registerReadEvent(clnt_sockfd, udata_ptr);
}

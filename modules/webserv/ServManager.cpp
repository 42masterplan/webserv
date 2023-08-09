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
 * @todo 종료시 열어둔 port 및 fd를 수거해줘야 합니다.
 */
ServManager::~ServManager() {
	for (size_t i = 0; i < serv_sock_fds_.size(); i++){
		close(serv_sock_fds_[i]);
	}
}


/**
 * @brief 
 * 
 */
void  ServManager::serverInit(){
	listen_ports_ = ConfParser::getInstance().getListenPorts();
	for (size_t i = 0; i < listen_ports_.size(); i++){
		sockInit();
		sockBind(listen_ports_[i]);
		sockListen();
		std::cout << "listen:::::"<<listen_ports_[i] << std::endl;
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
	int	optval;
	if (setsockopt(serv_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		throw(std::runtime_error("Socket opt Change ERROR!!"));
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
 * @brief listen()함수에 서버소켓과 15칸의 연결대기큐 사이즈 인자를 주어 호출합니다.
 * @note 여기서 서버 fd를 non-blocking으로 바꿔줍니다.
 * @exception listen()에서 에러 발생 시 runtime_error를 throw합니다.
 */
void  ServManager::sockListen(){
	if (listen(serv_sock_fds_[serv_sock_fds_.size() - 1], 15) == -1)
		throw(std::runtime_error("LISTEN() ERROR"));
	fcntl(serv_sock_fds_[serv_sock_fds_.size() - 1], F_SETFL, O_NONBLOCK);
}

/**
 * @brief 코어함수로, kqueue에서 받아온 이벤트들을 하나씩 처리합니다.
 * @exception read, write이외의 이벤트가 발생했을 시 runtime_error를 throw합니다.
 */
void  ServManager::handleEvents(){
	struct kevent*	cur_event;
  UData*					cur_udata;
	e_fd_type				cur_fd_type;

	for (int i = 0; i < event_list_size_; i++){
		cur_event = &event_list_[i];
		if (cur_event->udata != NULL){
			cur_udata = (UData*)cur_event->udata;
			cur_fd_type = cur_udata->fd_type_;
		}
		if (std::find(serv_sock_fds_.begin(), serv_sock_fds_.end(),cur_event->ident) != serv_sock_fds_.end())
			registerNewClnt(cur_event->ident);
		else if (cur_event->udata == NULL)
			continue;
		else if (cur_fd_type == CLNT && cur_event->filter == EVFILT_READ)
			EventHandler::getInstance().sockReadable(cur_event);
		else if (cur_fd_type == CLNT && cur_event->filter == EVFILT_WRITE)
			EventHandler::getInstance().sockWritable(cur_event);
		else if (cur_fd_type == CGI && cur_event->filter == EVFILT_READ)
			EventHandler::getInstance().cgiReadable(cur_event);
    else if (cur_fd_type == CGI && cur_event->filter == EVFILT_PROC)
      EventHandler::getInstance().cgiTerminated(cur_udata);
		else if (cur_fd_type == FILETYPE && cur_event->filter == EVFILT_READ)
			EventHandler::getInstance().fileReadable(cur_event);
		else if (cur_fd_type == FILETYPE && cur_event->filter == EVFILT_WRITE)
			EventHandler::getInstance().fileWritable(cur_event);
		else
			throw(std::runtime_error("????????THAT'S IMPOSSIBLE THIS IS CODE ERROR!!"));
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
	int									option;
	socklen_t						option_len;
	if (clnt_sockfd == -1)
		throw(std::runtime_error("ACCEPT() ERROR"));
	fcntl(clnt_sockfd, F_SETFL, O_NONBLOCK);
	option_len = sizeof(option);
	option = 1;
	setsockopt(clnt_sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&option, option_len);
	for (idx = 0; idx < serv_sock_fds_.size(); idx++){
		if (serv_sock_fds_[idx] == serv_sockfd)
			break;
	}
	UData*	udata_ptr = new UData(CLNT, listen_ports_[idx]);
	std::cout << "Connected Client : " << clnt_sockfd << std::endl;
	std::cout << "my_port:::" <<  listen_ports_[idx] << std::endl;
	udata_ptr->client_fd_ = clnt_sockfd;
	Kqueue::registerReadEvent(clnt_sockfd, udata_ptr);
}

std::string	ServManager::createSession(void) {
	const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  const int					length = 16;
  std::string				random_str;

  srand(static_cast<unsigned int>(time(NULL)));
	while (true) {
		random_str = "";
	  for (int i = 0; i < length; ++i)
	      random_str += characters[rand() % characters.length()];
		if (session.find(random_str) == session.end()) {
			session[random_str] = "user" + intToString(session.size());
		}
	}
	return random_str;
}

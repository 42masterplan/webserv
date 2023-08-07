#include "ServManager.hpp"

/**
 * @brief 싱글톤 패턴을 위해서 static 변수를 반환합니다.
 * @return ConfParser& 싱글톤 패턴을 위해서 static 변수를 반환합니다.
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
 * @brief 서버를 초기화 하는 함수로, 서버소켓 생성, 초기화, bind, listen의 과정으로 서버를 초기화합니다.
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

ServManager::ServManager(){}

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
			sockReadable(cur_event);
		else if (cur_fd_type == CLNT && cur_event->filter == EVFILT_WRITE)
			sockWritable(cur_event);
		else if (cur_fd_type == CGI && cur_event->filter == EVFILT_READ)
			cgiReadable(cur_event);
    else if (cur_fd_type == CGI && cur_event->filter == EVFILT_PROC)
      cgiTerminated(cur_udata);
		else if (cur_fd_type == FILETYPE && cur_event->filter == EVFILT_READ)
			fileReadable(cur_event);
		else if (cur_fd_type == FILETYPE && cur_event->filter == EVFILT_WRITE)
			fileWritable(cur_event);
		else{
			std:: cout << "????????" << cur_fd_type << "\n";
			throw(std::runtime_error("THAT'S IMPOSSIBLE THIS IS CODE ERROR!!"));
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
	struct sockaddr_in	clnt_addr;
	socklen_t						clnt_addrsz = sizeof(clnt_addr);
	int									clnt_sockfd = accept(serv_sockfd, (struct sockaddr *) &clnt_addr, &clnt_addrsz);
	int									option;
	socklen_t						optlen;
	if (clnt_sockfd == -1)
		throw(std::runtime_error("ACCEPT() ERROR"));
	std::cout << "Connected Client : " << clnt_sockfd << std::endl;
	fcntl(clnt_sockfd, F_SETFL, O_NONBLOCK);
	optlen = sizeof(option);
	option = 1;
	setsockopt(clnt_sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&option, optlen);
	UData*	udata_ptr = new UData(CLNT);
	udata_ptr->client_fd_ = clnt_sockfd;
	Kqueue::registerReadEvent(clnt_sockfd, udata_ptr);
}

/**
 * @brief 클라이언트 소켓이 readable할 때 호출되는 함수입니다.
 * @param cur_event 클라이언트 소켓에 해당되는 발생한 이벤트 구조체
 * @exception read()에서 에러 발생 시 runtime_error를 throw합니다.
 */
void  ServManager::sockReadable(struct kevent *cur_event){
	UData*	cur_udata = (UData*)cur_event->udata;
	if (cur_event->flags == EV_EOF){
		disconnectFd(cur_event);
		return;
	}
	if (cur_udata == NULL){
		std::cout << cur_event->ident << "is already disconnected!(read)"<< std::endl;
		return ;
	}

	std::vector<char>&	raw_data_ref = cur_udata->raw_data_;
	int rlen = read(cur_event->ident, buff_, BUFF_SIZE);
	if (rlen == -1){
		std::cerr << "already disconnected!"<< std::endl;
		// throw(std::runtime_error("READ() ERROR!! IN CLNT_SOCK"));
	} else if (rlen == 0){
		std::cout << "clnt sent eof. disconnecting.\n";
		disconnectFd(cur_event);
	}
	else{
		buff_[rlen] = '\0';
		raw_data_ref.insert(raw_data_ref.end(), buff_, buff_ + rlen);

		std::vector<HttpRequest>& http_request_ref = cur_udata->http_request_;
		if (http_request_ref.size() && (http_request_ref.back().getParseStatus() != FINISH && !http_request_ref.back().getRequestError()))
			http_request_ref.back().parse(raw_data_ref);
		while (http_request_ref.size() == 0 || (http_request_ref.back().getParseStatus() == FINISH && !http_request_ref.back().getRequestError())) {
			http_request_ref.push_back(HttpRequest());
			http_request_ref.back().parse(raw_data_ref);
		}
		// if (cur_udata->http_request_.size() == 0 || \
		// 	(cur_udata->http_request_.back().getParseStatus() == FINISH && cur_udata->raw_data_.size())){
		// 	HttpRequest request_parser;
		// 	cur_udata->http_request_.push_back(request_parser);
		// }
		// cur_udata->http_request_.back().parse(raw_data_ref);
		// cur_udata->http_request_.back().printBodyInfo();

		if (http_request_ref.back().getRequestError()) {
			//아에 잘못 된 형식으로 메세지가 온 경우들에 대해서 Request 단에서 에러를 처리해줍니다.
			//클라이언트 소켓 read_event 삭제 -> 파일 read_event 등록 -> 파일 read가 끝나면 그 파일을 write
			//어떤 파일을 가져와야하는지 확인해서 그 파일을 보낼 수 있도록 한다.
			//파일을 보내고 할 수 있는 선택 : 1. 연결을 끊는다. 또는 2.클래스에 담겨있는 정보들을 삭제한다.
			//HTTP status code를 정한다.
			//필요한 정보
			//1. status code
			//2. 해당하는 Server Block
			//TODO : error_handler 함수 만들기
			// errorHandler();
			return ;
		}
		if (raw_data_ref.size() == 0){
			cur_udata->http_response_.reserve(http_request_ref.size());
			for(size_t i = 0; i < http_request_ref.size(); i++){
				cur_udata->http_response_[i].makeResponse(http_request_ref[i]);
			}
			Kqueue::registerWriteEvent(cur_event->ident, cur_event->udata);
			Kqueue::unregisterReadEvent(cur_event->ident, cur_event->udata);//TODO: 나중에 Write Event가 끝나고 Udata delete 필요
		}
	}
}

/**
 * @brief 클라이언트 소켓이 writable할 때 호출되는 함수입니다.
 * @param cur_event 클라이언트 소켓에 해당되는 발생한 이벤트 구조체
 */
void  ServManager::sockWritable(struct kevent *cur_event){
	UData*	cur_udata = (UData*)cur_event->udata;
		if (cur_udata == NULL){
			std::cout << cur_event->ident << "is already disconnected!(Write)"<< std::endl;
		return ;
	}
	std::vector<char>&	ret_store_ref = cur_udata->ret_store_;
	if (!ret_store_ref.size())
    return ;
  int n = write(cur_event->ident, &ret_store_ref[0], ret_store_ref.size());
  if (n == -1){
    std::cerr << "client write error!" << std::endl;
    disconnectFd(cur_event);
	}
	else{
		ret_store_ref.erase(ret_store_ref.begin(), ret_store_ref.begin() + n);
		if (ret_store_ref.size() == 0){
			std::cout << "HI!!" << std::endl;
			Kqueue::registerReadEvent(cur_event->ident, cur_udata);
			Kqueue::unregisterWriteEvent(cur_event->ident, cur_udata);
		}
	}
}

/**
 * @brief cgi 파이프가 readable할 때 호출되는 함수입니다.
 * @param cur_event cgi 파이프에 해당되는 발생한 이벤트 구조체
 * @exception read()에서 에러 발생 시 runtime_error를 throw합니다.
 */
void  ServManager::cgiReadable(struct kevent *cur_event){
	UData*	cur_udata = (UData*)cur_event->udata;
	std::vector<char>&	raw_data_ref = cur_udata->raw_data_;

	int rlen = read(cur_event->ident, buff_, BUFF_SIZE);
	if (rlen == -1)
		throw(std::runtime_error("READ() ERROR!! IN CLNT_SOCK"));
	else if (rlen == 0){
		std::cout << "CGI process sent eof, closing fd.\n";
		if (cur_event->udata != NULL)
			disconnectFd(cur_event);
	}
	else{
		buff_[rlen] = '\0';
		raw_data_ref.insert(raw_data_ref.end(), buff_, buff_ + std::strlen(buff_));
    std::string raw_data_string = std::string(raw_data_ref.begin(), raw_data_ref.end());
    std::cout << "FROM CGI PROC\n" << raw_data_string << "\n";
    waitpid(-1, NULL, 0);
	}
}

/**
 * @brief CGI 프로세스를 회수하는 함수입니다.
 * @param udata pid가 담긴 udata입니다.
 * @exception 자식이 비정상적으로 종료된 것이 감지되면 runtime_error를 throw합니다.
 */
void  ServManager::cgiTerminated(UData* udata){
  int status;
  Kqueue::unregisterExitEvent(udata->cgi_pid_, udata);
  waitpid(udata->cgi_pid_, &status, 0);
  delete udata;
  if (WIFEXITED(status))
    return;
  else
    throw std::runtime_error("CGI terminated abnormally");
}

/**
 * @brief 파일을 Read하는 이벤트가 발생했을 때 해당하는 파일을 Read합니다.
 * @param cur_event 해당하는 이벤트에 해당하는 Udata가 들어있는 cur_event
 */
void  ServManager::fileReadable(struct kevent *cur_event){
	ssize_t read_len = read(cur_event->ident, buff_, BUFF_SIZE);
	UData*	cur_udata = (UData*)cur_event->udata;
	std::vector<char>& file_store_ref = cur_udata->file_read_write_store_;
	if (read_len <= 0){
		if (read_len == -1)
			cur_udata->status_code_ = 500;//this is error
		Kqueue::unregisterReadEvent(cur_event->ident, cur_udata);
		Kqueue::registerWriteEvent(cur_udata->client_fd_, cur_udata);
	}else{
		buff_[read_len] = '\0';
		file_store_ref.insert(file_store_ref.end(), buff_, buff_ + read_len);
	}
}

/**
 * @brief 파일에 Write하는 이벤트가 발생했을 때 해당하는 파일에 write합니다. 
 * @note Post에서 사용할 예정입니다.
 * @param cur_event 해당하는 이벤트에 해당하는 Udata가 들어있는 cur_event
 */
void	ServManager::fileWritable(struct kevent *cur_event){
	UData*	cur_udata = (UData*)cur_event->udata;
	std::vector<char> &write_store_ref = cur_udata->file_read_write_store_;
	int write_size = write(cur_event->ident, &write_store_ref[cur_udata->write_size_], write_store_ref.size() - cur_udata->write_size_);
	cur_udata->write_size_+= write_size;
	if ((size_t)cur_udata->write_size_ == write_store_ref.size()){
		cur_udata->status_code_ = 201;
		Kqueue::unregisterWriteEvent(cur_event->ident, cur_udata);
		Kqueue::registerWriteEvent(cur_udata->client_fd_, cur_udata);
	}
}

/**
 * @brief fd 연결을 끊는 함수
 * @param cur_event 해당되는 이벤트 구조체
 */
void  ServManager::disconnectFd(struct kevent *cur_event){
	UData*	udata = (UData*)cur_event->udata;
  if (udata->fd_type_ == CLNT)
	  std::cout << "CLIENT DISCONNECTED: " << cur_event->ident << std::endl;
  else if (udata->fd_type_ == CGI)
	  std::cout << "CGI PROCESS TERMINATED: " << udata->cgi_pid_ << std::endl;
	close(cur_event->ident);
	delete udata;
	cur_event->udata = NULL;
}

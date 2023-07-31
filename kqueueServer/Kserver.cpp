#include "Kserver.hpp"

/**
 * @brief Kserver 생성자입니다. 포트번호를 초기화합니다.
 * @param port 인자로 들어온 포트번호 char 포인터입니다.
 * @return 숫자가 아닌 글자가 들어올 경우 port_ 멤버변수에 -1을 저장하고 함수가 종료됩니다.
*/
Kserver::Kserver(char* port) {
	port_ = 0;
	if (port == NULL)
		port_ = -1;
	int num = 0;

	for (int i = 0; port[i] != '\0'; i++){
		num *= 10;
		if (!isdigit(port[i])){
			port_ = -1;
			return ;
		}
		num += (port[i] - '0');
	}
	if (num >= 65535) //포트 번호의 최대값을 넘어갈 경우
		port_ = -1;
	else
		port_ = num;
	std::cout << "Port : " << port_ << "\n";
}

/**
 * @brief Kserver 소멸자입니다. 서버 소켓을 닫습니다.
 * @todo 종료시 열어둔 port 및 fd를 수거해줘야 합니다.
 */
Kserver::~Kserver() { close(serv_sockfd_); }

/**
 * @brief 서버소켓을 초기화합니다. SO_REUSEADDR로 포트를 재사용하게 해주었습니다.
 * @exception 포트번호가 적절치 않거나 소켓 생성에 실패할 경우 throw 합니다.
 */
void  Kserver::sockInit(){
	if (port_ == -1)
		throw(std::invalid_argument("Port Num is not valid"));
	serv_sockfd_ = socket(PF_INET, SOCK_STREAM, 0);
	int	optval;
	setsockopt(serv_sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1;
	if (serv_sockfd_ == -1)
		throw(std::runtime_error("SOCK() ERROR"));
}

/**
 * @brief 서버의 serv_addr_ 변수를 초기화하고 bind()를 호출합니다.
 * @exception bind()에러 발생 시 runtime_error를 throw합니다.
 */
void  Kserver::sockBind(){
	struct sockaddr_in	serv_addr;

	std::memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port_);
	serv_addr.sin_family = AF_INET;
	if (bind(serv_sockfd_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
		throw(std::runtime_error("BIND() ERROR"));
}

/**
 * @brief listen()함수에 서버소켓과 15칸의 연결대기큐 사이즈 인자를 주어 호출합니다.
 * @note 여기서 서버 fd를 non-blocking으로 바꿔줍니다.
 * @exception listen()에서 에러 발생 시 runtime_error를 throw합니다.
 */
void  Kserver::sockListen(){
	if (listen(serv_sockfd_, 15) == -1)
		throw(std::runtime_error("LISTEN() ERROR"));
	fcntl(serv_sockfd_, F_SETFL, O_NONBLOCK);
}

/**
 * @brief 서버를 초기화 하는 함수로, 서버소켓 생성, 초기화, bind, listen의 과정으로 서버를 초기화합니다.
 */
void  Kserver::serverInit(){
	sockInit();
	sockBind();
	sockListen();
}

/**
 * @brief 메인로직을 구성하는 함수로, kqueue를 시작하고 무한루프를 돌면서 이벤트를 감지->처리합니다.
 */
void  Kserver::launchServer(){
	serv_udata_ptr_ = new UData(SERVER);
	kqueue_fd_ =  Kqueue::kqueueStart(serv_sockfd_, change_list_, serv_udata_ptr_);
	while (1){
		event_list_size_ = Kqueue::detectEvent(event_list_, change_list_, kqueue_fd_);
		handleEvents();
	}
}

/**
 * @brief 코어함수로, kqueue에서 받아온 이벤트들을 하나씩 처리합니다.
 * @exception read, write이외의 이벤트가 발생했을 시 runtime_error를 throw합니다.
 * @todo CGI분기문을 추가해야 합니다.
 */
void  Kserver::handleEvents(){
	struct kevent*	cur_event;
  UData*					cur_udata;
	e_fd_type				cur_fd_type;

	for (int i = 0; i < event_list_size_; i++){
		cur_event = &event_list_[i];
    cur_udata = (UData*)cur_event->udata;
		cur_fd_type = cur_udata->fd_type_;
		if (cur_fd_type == SERVER)
			registerNewClnt();
		else if (cur_fd_type == CLNT && cur_event->filter == EVFILT_READ)
			sockReadable(cur_event);
		else if (cur_fd_type == CLNT && cur_event->filter == EVFILT_WRITE)
			sockWriteable(cur_event);
		else {
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
void  Kserver::registerNewClnt(){
	struct sockaddr_in	clnt_addr;
	socklen_t						clnt_addrsz = sizeof(clnt_addr);
	int									clnt_sockfd = accept(serv_sockfd_, (struct sockaddr *) &clnt_addr, &clnt_addrsz);

	if (clnt_sockfd == -1)
		throw(std::runtime_error("ACCEPT() ERROR"));
	std::cout << "Connected Client : " << clnt_sockfd << "\n";
	fcntl(clnt_sockfd, F_SETFL, O_NONBLOCK);
	UData*	udata_ptr = new UData(CLNT);
	Kqueue::ChangeEvent(clnt_sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, udata_ptr, change_list_);
	Kqueue::ChangeEvent(clnt_sockfd, EVFILT_WRITE, EV_ADD | EV_ENABLE, udata_ptr, change_list_);
}

/**
 * @brief 클라이언트 소켓이 readable할 때 호출되는 함수입니다.
 * @param cur_event 클라이언트 소켓에 해당되는 발생한 이벤트 구조체
 * @exception read()에서 에러 발생 시 runtime_error를 throw합니다.
 */
void  Kserver::sockReadable(struct kevent *cur_event){
	UData*	cur_udata = (UData*)cur_event->udata;
	std::vector<char>&	raw_data_ref = cur_udata->raw_data_;

	int rlen = read(cur_event->ident, buff_, BUFF_SIZE);
	if (rlen == -1)
		throw(std::runtime_error("READ() ERROR!! IN CLNT_SOCK"));
	else if (rlen == 0){
		std::cout << "clnt sent eof. disconnecting.\n";
		disconnectClient(cur_event);
	}
	else{
		buff_[rlen] = '\0';
		raw_data_ref.insert(raw_data_ref.end(), buff_, buff_ + std::strlen(buff_));
		std::cout << "FROM CLIENT NUM " << cur_event->ident << ": " << std::string(raw_data_ref.begin(), raw_data_ref.end()) << "\n";
	}
}

/**
 * @brief 클라이언트 소켓이 writable할 때 호출되는 함수입니다.
 * @param cur_event 클라이언트 소켓에 해당되는 발생한 이벤트 구조체
 */
void  Kserver::sockWriteable(struct kevent *cur_event){
	UData*	cur_udata = (UData*)cur_event->udata;
	std::vector<char>&	raw_data_ref = cur_udata->raw_data_;

	if (raw_data_ref.size()){
		char* buff = new char[raw_data_ref.size()];
		std::copy(raw_data_ref.begin(), raw_data_ref.end(), buff);
    int n = write(cur_event->ident, buff, std::strlen(buff));
		delete[] buff;
    std::cout << "Writeable\n";
    if (n == -1){
        std::cerr << "client write error!" << "\n";
        disconnectClient(cur_event);
    }
    else
        raw_data_ref.clear();
	}
}

/**
 * @brief 클라이언트 연결을 끊는 함수
 * @param clnt_fd 연결을 끊을 클라이언트 fd
 */
void  Kserver::disconnectClient(struct kevent *cur_event){
	UData*	udata = (UData*)cur_event->udata;
	delete udata;
	std::cout << "CLIENT DISCONNECTED: " << cur_event->ident << "\n";
	close(cur_event->ident);
}

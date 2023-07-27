#include "Kserver.hpp"

Kqueue::Kqueue(){}
Kqueue::~Kqueue() { close(kqueue_fd_); }

/**
 * @brief kserver 생성자(char* Port)
 *
 * @param port
 * 포트번호 오류 시 -1을 port 멤버에 초기화
 * @warning 중복 포트 번호 사용시 꺼짐
*/
Kserver::Kserver(char* port) {
	port_ = 0;
	if (port == NULL)
		port_ = -1;
	int num = 0;
	int i;

	for (i=0; port[i] != '\0';i++){
		num *= 10;
		if (!isdigit(port[i])){
			port_ = -1;
			return ;
		}
		num += (port[i] - '0');
	}
	if (i >= 6)
		port_ = -1;
	else
		port_ = num;
	std::cout << "Port : " << port_ << "\n";
}

/**
 * @brief Destroy the Kserver:: Kserver object
 * @todo 종료시 열어둔 port 및 fd를 수거해줘야 합니다.
 *
 */
Kserver::~Kserver() {
	close(serv_sockfd_);
}

/**
 * @brief sock(), bind(), listen()
 * @warning it can throw runtime_error, invalid_argument
 *
 */
void  Kserver::serverInit(){
	sockInit();
	sockBind();
	sockListen();
}

/**
 * @brief accept 함수 호출합니다.
 * @note 호출된 clnt_sockfd에 accept된 fd가 있어서 그것들에 대한 처리가 필요합니다. (현재 사용 X)
 *
*/
// void  Kserver::sockAccept(){
// 	clnt_addrsz_ = sizeof(clnt_addr_);
// 	clnt_sockfd_ = accept(serv_sockfd_, (struct sockaddr *) &clnt_addr_, &clnt_addrsz_);
// 	if (clnt_sockfd_ == -1)
// 		throw(std::runtime_error("ACCEPT() ERROR"));
// 	std::cout << "Connected Client : " << clnt_sockfd_ << "\n";
// }

/**
 * @brief 메인로직 kqueue를 시작하고 무한 루프를 돌면서 이벤트를 감지->처리합니다.
 *
 */
void  Kserver::startWorking(){
	kqueue_.KqueueStart(serv_sockfd_);
	while (1){
		event_list_size_ = kqueue_.detectEvent(event_list_);
		handleEvents();
	}
}

/**
 * @brief socket(PF_INET, SOCK_STREAM, 0) :서버 소켓을 만듭니다.
 * @exception invalid_argument 포트가 틀린경우
 *
 */
void  Kserver::sockInit(){
	if (port_ == -1)
		throw(std::invalid_argument("Port Num is not valid"));
	serv_sockfd_ = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sockfd_ == -1)
		throw(std::runtime_error("SOCK() ERROR"));
}

/**
 * @brief 서버의 struct sockaddr_in serv_addr
 변수를 초기화 하고 Bind()를 호출합니다.
 *
 */
void  Kserver::sockBind(){
	std::memset(&serv_addr_,0,sizeof(serv_addr_));
	serv_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr_.sin_port = htons(port_);
	serv_addr_.sin_family = AF_INET;
	if (bind(serv_sockfd_, (struct sockaddr *)&serv_addr_, sizeof(serv_addr_)) == -1)
		throw(std::runtime_error("BIND() ERROR"));
}

/**
 * @brief listen() 연결 요청 대기 큐 15개
 * @note 여기서 서버 fd를 non-blocking으로 바꿔줍니다.
 *
 */
void  Kserver::sockListen(){
	if (listen(serv_sockfd_, 15) == -1)
		throw(std::runtime_error("LISTEN() ERROR"));
	fcntl(serv_sockfd_, F_SETFL, O_NONBLOCK);
}

/**
 * @brief 코어함수! 여기서 kqueue에서 받아온 이벤트들을 하나씩 처리해준다.
 *
 */
void  Kserver::handleEvents(){
	struct kevent *cur_event;
	for(int i=0;i < event_list_size_;i++){
		cur_event = &event_list_[i];
		if (cur_event->filter ==  EVFILT_READ){
			if ((int)cur_event->ident == serv_sockfd_)
				registerNewClnt();
			else//client-socket
				sockReadable(cur_event);
		}
		else if (cur_event->filter == EVFILT_WRITE)
			sockWriteable(cur_event);//ONLY CLIENT
		else{
			std:: cout << "????????" <<  cur_event->filter << "\n";
			throw(std::runtime_error("THAT'S IMPOSSIBLE THIS IS CODE ERROR!!"));
		}
	}
}

/**
 * @brief 새로운 클라이언트를 받았다고 생각한 함수 클라이언트 소켓을 accept 해주고, nonblocking으로 만든 후, kqueue 이벤트에 read,write 모두 등록해줍니다.
 * 추가적으로 클라이언트 저장소에 빈 문자열로 저장해줍니다.
 *
 */
void  Kserver::registerNewClnt(){
	clnt_addrsz_ = sizeof(clnt_addr_);
	clnt_sockfd_ = accept(serv_sockfd_,  (struct sockaddr *) &clnt_addr_, &clnt_addrsz_);
	if (clnt_sockfd_ == -1)
		throw(std::runtime_error("ACCEPT() ERROR"));
	std::cout << "Connected Client : " << clnt_sockfd_ << "\n";
	fcntl(clnt_sockfd_, F_SETFL, O_NONBLOCK);
	kqueue_.ChangeEvent(clnt_sockfd_, EVFILT_READ, EV_ADD | EV_ENABLE, NULL);
	kqueue_.ChangeEvent(clnt_sockfd_, EVFILT_WRITE, EV_ADD | EV_ENABLE, NULL);
	clnt_store_[clnt_sockfd_] = "";
}

/**
 * @brief client socket is in a readable state
 *
 * @param cur_event 클라이언트 소켓에 해당되는 발생한 이벤트 구조체
 */
void  Kserver::sockReadable(struct kevent *cur_event){
	if (clnt_store_.find(cur_event->ident) == clnt_store_.end()){
		std::cout << "Already disconnected\n";
		return ;
	}
	int rlen = read(cur_event->ident ,buff_, BUFF_SIZE);
	if (rlen == -1)
		throw(std::runtime_error("READ() ERROR!! IN CLNT_SOCK"));
	if (rlen == 0){
		std::cout << "clnt shoot eof\n";
		disconnectClient(cur_event->ident);
	}
	else{
		buff_[rlen] = '\0';
		clnt_store_[cur_event->ident] += buff_;
		std::cout << "FROM CLIENT NUM " << cur_event->ident << ": " << clnt_store_[cur_event->ident] << "\n";
	}
}

/**
 * @brief clnt socket is writable state
 *
 * @param cur_event 클라이언트 소켓에 해당되는 발생한 이벤트 구조체
 */
void  Kserver::sockWriteable(struct kevent *cur_event){
	if (clnt_store_.find(cur_event->ident) != clnt_store_.end()){
		if (clnt_store_[cur_event->ident] != ""){
				int n = write(cur_event->ident, clnt_store_[cur_event->ident].c_str(),clnt_store_[cur_event->ident].size());
				std::cout << "Writeable\n";
				if (n == -1){
						std::cerr << "client write error!" << "\n";
						disconnectClient(cur_event->ident);
				}
				else
						clnt_store_[cur_event->ident].clear();
		}
	}
}

/**
 * @brief 클라이언트 연결을 끊는 함수
 *
 * @param clnt_fd 연결을 끊을 클라이언트 fd
 */
void  Kserver::disconnectClient(int clnt_fd){
	std::cout << "CLIENT DISCONNECTED:: " << clnt_fd << "\n";
	// kqueue_.ChangeEvent(clnt_fd, EVFILT_READ, EV_DELETE | EV_DISABLE, NULL);
	// kqueue_.ChangeEvent(clnt_fd,EVFILT_WRITE, EV_DELETE | EV_DISABLE, NULL);
	close(clnt_fd);
	clnt_store_.erase(clnt_fd);
}

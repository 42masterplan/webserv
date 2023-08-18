
# include "EventHandler.hpp"
EventHandler::~EventHandler(){}
/**
 * @brief 싱글톤 패턴을 위해서 static 변수를 반환합니다.
 * @return event_handler& 싱글톤 패턴을 위해서 static 변수를 반환합니다.
 */
EventHandler& EventHandler::getInstance(){
	static EventHandler event_handler;
	return event_handler;
}

/**
 * @brief 클라이언트 소켓이 readable할 때 호출되는 함수입니다.
 * @param cur_event 클라이언트 소켓에 해당되는 발생한 이벤트 구조체
 * @exception read()에서 에러 발생 시 runtime_error를 throw합니다.
 */
void  EventHandler::sockReadable(struct kevent *cur_event){
	// std::cout << "SOCK Readable" << std::endl;
	UData*	udata = (UData*)cur_event->udata;
	if (cur_event->flags == EV_EOF){
		disconnectFd(cur_event);
		return;
	}
	if (udata == NULL){ // TODO: 제출 시 삭제 예정
		std::cout << cur_event->ident << "is already disconnected! (read)" << std::endl;
		// disconnectFd(cur_event);
		return ;
	}

	std::vector<char>&	raw_data_ref = udata->raw_data_;
	int rlen = read(cur_event->ident, buff_, BUFF_SIZE);
	if (rlen == -1){
		std::cout << "플래그다"<< cur_event->flags << std::endl;
		if (EV_ERROR & cur_event->flags)
			std::cout << "에러가 발생했습니다~" <<std::endl;
		std::cerr << "sock read fail! (READ)"<< std::endl;
		disconnectFd(cur_event);
		return ;
	}
	else if (rlen == 0){
		std::cout << "clnt sent eof. disconnecting." << std::endl;
		disconnectFd(cur_event);
		return ;
	}
	else{
		raw_data_ref.insert(raw_data_ref.end(), buff_, buff_ + rlen);
		std::vector<HttpRequest>& request_ref = udata->http_request_;
		if (request_ref.size() && (request_ref.back().getParseStatus() != FINISH && !request_ref.back().getRequestError()))
			request_ref.back().parse(raw_data_ref);
		while (request_ref.size() == 0 || (request_ref.back().getParseStatus() == FINISH && !request_ref.back().getRequestError())) {
			request_ref.push_back(HttpRequest());
			request_ref.back().setPort(udata->port_);
			request_ref.back().parse(raw_data_ref);
		}
		if (request_ref.back().getRequestError() != OK){ //에러페이지 설정 때문에 앞으로 뺐습니다.
			udata->http_response_ = HttpResponse(request_ref[0]);
		}
		switch (request_ref.back().getRequestError()){
			case (OK) : 
				break;
			case (FORM_ERROR) :
				return HttpResponseHandler::getInstance().errorCallBack(*udata, 404);
			case (METHOD_ERROR) :
				return HttpResponseHandler::getInstance().errorCallBack(*udata, 405);
			case (VERSION_ERROR) :
				return HttpResponseHandler::getInstance().errorCallBack(*udata, 505);
			case (UNIMPLEMENTED_ERROR) :
				return HttpResponseHandler::getInstance().errorCallBack(*udata, 501);
			case (LENGTH_REQUIRED_ERROR) :
				return HttpResponseHandler::getInstance().errorCallBack(*udata, 411);
		}
		if (request_ref.size() != 0 && request_ref.front().getParseStatus() == FINISH)
			HttpResponseHandler::getInstance().parseResponse(udata);
	}
}

/**
 * @brief 클라이언트 소켓이 writable할 때 호출되는 함수입니다.
 * @param cur_event 클라이언트 소켓에 해당되는 발생한 이벤트 구조체
 */
void  EventHandler::sockWritable(struct kevent *cur_event){
	// std::cout << "SOCK Writable" << std::endl;
	UData*	udata = (UData*)cur_event->udata;
	if (udata == NULL){
		std::cout << cur_event->ident << "is already disconnected!(Write)"<< std::endl;
		return ;
	}

	std::vector<char>&	head_ref = udata->http_response_.getJoinedData();
	std::vector<char>&	body_ref = udata->http_response_.getBody();
	if (head_ref.size()) {// 여기가 첫번째 요청을 보내는 곳
		if (!writeToclient(head_ref, false, udata))
				disconnectFd(cur_event);
	}
	else  {// 두번째 body를 보내는 분기입니다.
		if (!writeToclient(body_ref, true, udata))
			disconnectFd(cur_event);
	}
}


/**
 * @brief cgi 파이프가 readable할 때 호출되는 함수입니다.
 * @param cur_event cgi 파이프에 해당되는 발생한 이벤트 구조체
 * @exception read()에서 에러 발생 시 runtime_error를 throw합니다.
 */
void  EventHandler::cgiReadable(struct kevent *cur_event){
	UData*	udata = (UData*)cur_event->udata;
	HttpResponse &res = udata->http_response_;
	
	int rlen = read(cur_event->ident, buff_, BUFF_SIZE);
  // std::cout << "MESSAGE FROM CGI:";
  // std::cout << buff_ << "\n";
	if (rlen == -1)
		throw(std::runtime_error("READ() ERROR!! IN CLNT_SOCK"));
	else if (rlen == 0){
		std::cout << "CGI process sent eof.\n";
    udata->fd_type_ = CLNT;
    waitpid(udata->cgi_pid_, NULL, 0);
		close(cur_event->ident);
		res.header_complete_ = false;
		if (!res.makeCgiResponse()){
			res.joined_data_.clear();
			res.body_.clear();
			return HttpResponseHandler::getInstance().errorCallBack(*udata, 500);
		}
		Kqueue::registerWriteEvent(udata->client_fd_, udata);
  }
	else{
		std::vector<char>& buff_ref = res.header_complete_ ? res.body_ : res.joined_data_;
		buff_ref.insert(buff_ref.end(), buff_, buff_ + rlen);
		// 헤더에 넣었으면 헤더 끝났는지 확인
		if (!res.header_complete_) {
			const char* header_end = strstr(&buff_ref[0], "\r\n\r\n");
			if (header_end) {
					res.header_complete_ = true;
					size_t header_size = header_end - &buff_ref[0] + 4;
					res.body_.assign(buff_ref.begin() + header_size, buff_ref.end());
					buff_ref.erase(buff_ref.begin() + header_size, buff_ref.end());
			}
		}
	}
}

/**
 * @brief cgi 파이프가 Writable할 때 호출되는 함수입니다.
 * @param cur_event cgi 파이프에 해당되는 발생한 이벤트 구조체
 */
void  EventHandler::cgiWritable(struct kevent *cur_event){
	std::cout << "CGI Writable" << std::endl;
	UData*	udata = (UData*)cur_event->udata;
	const std::vector<char> &write_store_ref = udata->http_request_[0].getBody();
  // print_vec(write_store_ref);
	// print_vec(write_store_ref);
	int write_size = write(cur_event->ident, &write_store_ref[udata->write_size_], write_store_ref.size() - udata->write_size_);
	if (write_size == -1) {// 실패하면 코드가 이상하긴 하다.
		udata->write_size_= 0;
		close(cur_event->ident);
		return ;
	}
	udata->write_size_+= write_size;
	if ((size_t)udata->write_size_ == write_store_ref.size()){
		close(cur_event->ident); //unregister?
		// Kqueue::registerReadEvent(udata->r_pfd, udata);
		udata->write_size_ = 0;
	}
}

/**
 * @brief cgi 프로세스가 타임아웃 되었을 때 호출되는 함수입니다.
 * @param cur_event cgi 프로세스에 해당되는 발생한 이벤트 구조체
 */
void  EventHandler::cgiTimeout(struct kevent *cur_event){
  UData*	udata = (UData*)cur_event->udata;
  std::cout << "CGI TIMEDOUT, KILL:" << udata->cgi_pid_ << std::endl;
  kill(udata->cgi_pid_, SIGKILL);
}

/**
 * @brief 파일을 Read하는 이벤트가 발생했을 때 해당하는 파일을 Read합니다.
 * @param cur_event 해당하는 이벤트에 해당하는 Udata가 들어있는 cur_event
 */
void  EventHandler::fileReadable(struct kevent *cur_event){
	std::cout << "FILE READable" << std::endl;
	ssize_t read_len = read(cur_event->ident, buff_, BUFF_SIZE);
	UData*	udata = (UData*)cur_event->udata;
	std::vector<char>& file_store_ref = udata->http_response_.getBody();
	if (read_len == -1 )
		return;
	if (udata->http_response_.file_size_ < static_cast<long>(read_len))//읽고 있는 파일을 삭제하는 경우 또는 파일크기보다 갑자기 더 큰게 읽히면 에러로 처리
		return fileErrorCallBack(cur_event);
	// buff_[read_len] = '\0';
	file_store_ref.insert(file_store_ref.end(), buff_, buff_ + read_len);
	// print_vec(file_store_ref);
	udata->http_response_.file_size_ -= read_len;
	if (udata->http_response_.file_size_ == 0){
		std::cout << "파일 다읽었어요~" <<std::endl;
		close(cur_event->ident);
		udata->fd_type_= CLNT;
		udata->http_response_.setContentLength(file_store_ref.size());
		udata->http_response_.makeBodyResponse(udata->http_request_[0]);
		Kqueue::registerWriteEvent(udata->client_fd_, udata);
	}
}


/**
 * @brief 파일에 Write하는 이벤트가 발생했을 때 해당하는 파일에 write합니다.
 * @note Post에서 사용할 예정입니다.
 * @param cur_event 해당하는 이벤트에 해당하는 Udata가 들어있는 cur_event
 */
void	EventHandler::fileWritable(struct kevent *cur_event){//TODO: Max_body_size 어디서 처리할지 정하기.
	std::cout << "FILE Writable" << std::endl;
	UData*	udata = (UData*)cur_event->udata;
	const std::vector<char> &write_store_ref = udata->http_request_[0].getBody();
	int write_size = write(cur_event->ident, &write_store_ref[udata->write_size_], write_store_ref.size() - udata->write_size_);
	if (write_size == -1) //실패하면 코드가 이상하긴 하다.
		return fileErrorCallBack(cur_event);
	udata->write_size_+= write_size;
	if ((size_t)udata->write_size_ == write_store_ref.size()){
		udata->http_response_.setStatusCode(201);
		udata->http_response_.setContentLength(0);
		udata->http_response_.makeBodyResponse(udata->http_request_[0]);
		udata->fd_type_ = CLNT;
		close(cur_event->ident);
		Kqueue::registerWriteEvent(udata->client_fd_, udata);
		udata->write_size_ = 0;
	}
}


/**
 * @brief fd 연결을 끊는 함수
 * @param cur_event 해당되는 이벤트 구조체
 */
void  EventHandler::disconnectFd(struct kevent *cur_event){
	UData*	udata = (UData*)cur_event->udata;
	if (udata){
		if (udata->is_forked_){
			kill(udata->cgi_pid_, SIGKILL);
			close(udata->r_pfd);
			close(udata->w_pfd);
		}
		close(udata->client_fd_);
	  if (udata->fd_type_ == CLNT)
		  std::cout << "CLIENT DISCONNECTED: " << cur_event->ident << std::endl;
	  else if (udata->fd_type_ == CGI)
		  std::cout << "CGI FD DISCONNECTED: " << cur_event->ident << std::endl;
		delete udata;
		cur_event->udata = NULL;
	}
	close(cur_event->ident);
}

/**
 * @brief 클라이언트에게 write하는 함수
 * 
 * @param to_write 보낼 벡터
 * @param is_body 바디인지 아닌지 확인
 * @param udata udata
 */
bool	EventHandler::writeToclient(std::vector<char> &to_write, bool is_body, UData*	udata){
	int n;
	int w_size = udata->write_size_;
	if (to_write.size() < (size_t) w_size){
		std::cout << to_write.size() << "|" << w_size<<std::endl;
		std::cout << "말이 안돼!" <<std::endl;
		return false;
	}
	n = write(udata->client_fd_, &to_write[w_size], to_write.size() - w_size);
	udata->write_size_ += n;
	if (n == -1)
		return false;
	else if ((size_t)udata->write_size_ == to_write.size() || to_write.size() == 0){
		udata->write_size_ = 0;
		if (!is_body){
			std::cout << "헤더 보냈어요" <<std::endl;
			to_write.clear();
		}
		else{
			std::cout << "바디 보냈어요" <<std::endl;
			std::cout << "-----------------------" <<std::endl<<std::endl;
			Kqueue::unregisterWriteEvent(udata->client_fd_, udata);
			udata->http_request_.erase(udata->http_request_.begin());
			if (udata->http_request_.size() != 0 && udata->http_request_[0].getParseStatus() == FINISH)
				HttpResponseHandler::getInstance().parseResponse(udata);
			else
				Kqueue::registerReadEvent(udata->client_fd_, udata);
		}
	}
	return true;
}

void	EventHandler::fileErrorCallBack(struct kevent *cur_event){
	close(cur_event->ident);
	std::cout << "파일에서 에러가 났어용~~"<<std::endl;
	HttpResponseHandler::getInstance().errorCallBack(*(UData *)(cur_event->udata), 500);
}

EventHandler::EventHandler(){}

/**
 * @brief CGI 프로세스를 회수하는 함수입니다.
 * @param udata pid가 담긴 udata입니다.
 * @exception 자식이 비정상적으로 종료된 것이 감지되면 runtime_error를 throw합니다.
 */
// void  EventHandler::cgiTerminated(UData* udata){
//   int status;
// 	std::cout << "CGI PROCESS TERMINATED: " << udata->cgi_pid_ << std::endl;
  
//   waitpid(udata->cgi_pid_, &status, 0);
//   udata->prog_name_ = "";
//   udata->cgi_pid_ = 0;
//   if (WIFEXITED(status))
//     return;
//   else
//     throw std::runtime_error("CGI terminated abnormally");
// }

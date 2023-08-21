
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
 * @param event 클라이언트 소켓에 해당되는 발생한 이벤트 구조체
 * @exception read()에서 에러 발생 시 runtime_error를 throw합니다.
 */
void  EventHandler::sockReadable(struct kevent *event){
	UData*	udata = (UData*)event->udata;
	if (event->flags == EV_EOF)
		return disconnectFd(event);
	if (udata == NULL)
		return ;
	std::vector<char>&	raw_data_ref = udata->raw_data_;
	int rlen = read(event->ident, buff_, BUFF_SIZE);
	if (rlen == -1)
		return disconnectFd(event);
	else if (rlen == 0)
		return disconnectFd(event);
	else{
		raw_data_ref.insert(raw_data_ref.end(), buff_, buff_ + rlen);
		std::vector<HttpRequest>& request_ref = udata->http_request_;
		if (request_ref.size() && (request_ref.back().getParseStatus() != FINISH && !request_ref.back().getRequestError()))
			request_ref.back().parse(raw_data_ref);
		while (request_ref.size() == 0 || (request_ref.back().getParseStatus() == FINISH && !request_ref.back().getRequestError())) {
			request_ref.push_back(HttpRequest());
			request_ref.back().setPort(udata->port_);
			std::cout << BLUE << "Parse [" << udata->client_fd_ << "]\'s Request" << CLOSE << std::endl;
			request_ref.back().parse(raw_data_ref);
		}
		if (request_ref.back().getRequestError() != OK) //에러페이지 설정 때문에 앞으로 뺐습니다.
			udata->http_response_ = HttpResponse(request_ref.back());
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
		if (request_ref.size() != 0 && request_ref.front().getParseStatus() == FINISH){
			std::cout << BLUE << "Parse [" << udata->client_fd_ << "] finished! Started to make Response ..." << CLOSE << std::endl;
			HttpResponseHandler::getInstance().parseResponse(udata);
		}
	}
}

/**
 * @brief 클라이언트 소켓이 writable할 때 호출되는 함수입니다.
 * @param event 클라이언트 소켓에 해당되는 발생한 이벤트 구조체
 */
void  EventHandler::sockWritable(struct kevent *event){
	UData*	udata = (UData*)event->udata;
	if (udata == NULL)
		return ;
	std::vector<char>&	head_ref = udata->http_response_.getJoinedData();
	std::vector<char>&	body_ref = udata->http_response_.getBody();
	if (head_ref.size()) {// 여기가 첫번째 요청을 보내는 곳
		if (!writeToclient(head_ref, false, udata))
				disconnectFd(event);
	}
	else  {// 두번째 body를 보내는 분기입니다.
		if (!writeToclient(body_ref, true, udata))
			disconnectFd(event);
	}
}


/**
 * @brief cgi 파이프가 readable할 때 호출되는 함수입니다.
 * @param event cgi 파이프에 해당되는 발생한 이벤트 구조체
 * @exception read()에서 에러 발생 시 runtime_error를 throw합니다.
 */
void  EventHandler::cgiReadable(struct kevent *event){
	UData*	udata = (UData*)event->udata;
	HttpResponse &res = udata->http_response_;
  Kqueue::unregisterTimeoutEvent(udata->cgi_pid_, udata);
  Kqueue::registerTimeoutEvent(udata->cgi_pid_, udata);
	int rlen = read(event->ident, buff_, BUFF_SIZE);
	if (rlen == -1){
		Kqueue::unregisterTimeoutEvent(udata->cgi_pid_, udata);
		close(udata->w_pfd);
		close(udata->r_pfd);
		return HttpResponseHandler::getInstance().errorCallBack(*udata, 500);
	}
	else if (rlen == 0){
    Kqueue::unregisterTimeoutEvent(udata->cgi_pid_, udata);
    udata->fd_type_ = CLNT;
    waitpid(udata->cgi_pid_, NULL, 0);
		close(event->ident);
		res.header_complete_ = false;
		udata->is_forked_ = false;
		if (!res.makeCgiResponse()){
			res.joined_data_.clear();
			res.body_.clear();
			return HttpResponseHandler::getInstance().errorCallBack(*udata, 500);
		}
		udata->write_size_ = 0;
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
 * @param event cgi 파이프에 해당되는 발생한 이벤트 구조체
 */
void  EventHandler::cgiWritable(struct kevent *event){

	UData*	udata = (UData*)event->udata;
	const std::vector<char> &write_store_ref = udata->http_request_[0].getBody();
  Kqueue::unregisterTimeoutEvent(udata->cgi_pid_, udata);
  Kqueue::registerTimeoutEvent(udata->cgi_pid_, udata);
	int write_size = write(event->ident, &write_store_ref[udata->write_size_], write_store_ref.size() - udata->write_size_);
	if (write_size == -1) {
		udata->write_size_= 0;
		close(event->ident);
		return ;
	}
	udata->write_size_+= write_size;
	if ((size_t)udata->write_size_ == write_store_ref.size()){
		close(event->ident);
		udata->write_size_ = 0;
	}
}

/**
 * @brief cgi 프로세스가 타임아웃 되었을 때 호출되는 함수입니다.
 * @param event cgi 프로세스에 해당되는 발생한 이벤트 구조체
 */
void  EventHandler::cgiTimeout(struct kevent *event){
  UData*	udata = (UData*)event->udata;
  std::cout << RED << "CGI TIMEDOUT, KILL:" << udata->cgi_pid_ << CLOSE << std::endl;
  kill(udata->cgi_pid_, SIGKILL);
}

/**
 * @brief 파일을 Read하는 이벤트가 발생했을 때 해당하는 파일을 Read합니다.
 * @param event 해당하는 이벤트에 해당하는 Udata가 들어있는 event
 */
void  EventHandler::fileReadable(struct kevent *event){
	ssize_t read_len = read(event->ident, buff_, BUFF_SIZE);
	UData*	udata = (UData*)event->udata;
	std::vector<char>& file_store_ref = udata->http_response_.getBody();
	if (read_len == -1 )
		return;
	if (udata->http_response_.file_size_ < static_cast<long>(read_len))//읽고 있는 파일을 삭제하는 경우 또는 파일크기보다 갑자기 더 큰게 읽히면 에러로 처리
		return fileErrorCallBack(event);
	file_store_ref.insert(file_store_ref.end(), buff_, buff_ + read_len);
	udata->http_response_.file_size_ -= read_len;
	if (udata->http_response_.file_size_ == 0){
		std::cout << GREEN << "File Read Done" << CLOSE << std::endl;
		close(event->ident);
		udata->fd_type_= CLNT;
		udata->http_response_.setContentLength(file_store_ref.size());
		udata->http_response_.makeBodyResponse();
		Kqueue::registerWriteEvent(udata->client_fd_, udata);
	}
}


/**
 * @brief 파일에 Write하는 이벤트가 발생했을 때 해당하는 파일에 write합니다.
 * @note Post에서 사용할 예정입니다.
 * @param event 해당하는 이벤트에 해당하는 Udata가 들어있는 event
 */
void	EventHandler::fileWritable(struct kevent *event){
	UData*	udata = (UData*)event->udata;
	const std::vector<char> &write_store_ref = udata->http_request_[0].getBody();
	int write_size = write(event->ident, &write_store_ref[udata->write_size_], write_store_ref.size() - udata->write_size_);
	if (write_size == -1)
		return fileErrorCallBack(event);
	udata->write_size_+= write_size;
	if ((size_t)udata->write_size_ == write_store_ref.size()){
		udata->http_response_.setStatusCode(201);
		udata->http_response_.setContentLength(0);
		udata->http_response_.makeBodyResponse();
		udata->fd_type_ = CLNT;
		close(event->ident);
		Kqueue::registerWriteEvent(udata->client_fd_, udata);
		udata->write_size_ = 0;
	}
}


/**
 * @brief fd 연결을 끊는 함수
 * @param event 해당되는 이벤트 구조체
 */
void  EventHandler::disconnectFd(struct kevent *event){
	UData*	udata = (UData*)event->udata;
	if (udata){
		if (udata->is_forked_){
			kill(udata->cgi_pid_, SIGKILL);
			close(udata->r_pfd);
			close(udata->w_pfd);
		}
		close(udata->client_fd_);
	  if (udata->fd_type_ == CLNT)
		  std::cout << LIGHT_YELLOW << "CLIENT DISCONNECTED: " << event->ident << CLOSE << std::endl;
	  else if (udata->fd_type_ == CGI)
		  std::cout << LIGHT_YELLOW << "CGI FD DISCONNECTED: " << event->ident << CLOSE << std::endl;
		delete udata;
		event->udata = NULL;
	}
	close(event->ident);
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
	if (to_write.size() < (size_t) w_size)
		return false;

	n = write(udata->client_fd_, &to_write[w_size], to_write.size() - w_size);
	udata->write_size_ += n;
	if (n == -1)
		return false;
	else if ((size_t)udata->write_size_ == to_write.size() || to_write.size() == 0){
		udata->write_size_ = 0;
		if (!is_body){
			std::cout << GREEN << "Send Header to [" << udata->client_fd_ << "]" << CLOSE << std::endl;
			print_vec(to_write);
			std::cout << std::endl;
			to_write.clear();
		}
		else{
			std::cout << GREEN << "Send Body to [" << udata->client_fd_ << "]" << CLOSE << std::endl;
			std::cout << "---------------------------------"<< std::endl;
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

void	EventHandler::fileErrorCallBack(struct kevent *event){
	close(event->ident);
	std::cout << RED << "File Error occurred!" << CLOSE << std::endl;
	HttpResponseHandler::getInstance().errorCallBack(*(UData *)(event->udata), 500);
}

EventHandler::EventHandler(){}

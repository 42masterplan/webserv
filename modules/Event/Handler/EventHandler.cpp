
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
	UData*	cur_udata = (UData*)cur_event->udata;
	if (cur_event->flags == EV_EOF){
		disconnectFd(cur_event);
		return;
	}
	if (cur_udata == NULL){
		std::cout << cur_event->ident << "is already disconnected!(read)" << std::endl;
		// disconnectFd(cur_event);
		return ;
	}

	std::vector<char>&	raw_data_ref = cur_udata->raw_data_;
	int rlen = read(cur_event->ident, buff_, BUFF_SIZE);
	if (rlen == -1){
		std::cerr << "already disconnected!(READ)"<< std::endl;
		delete (UData*) cur_event->udata;
		cur_event->udata = NULL;
		disconnectFd(cur_event);
		return ;
	}
	else if (rlen == 0){
		std::cout << "clnt sent eof. disconnecting." << std::endl;
		delete (UData*) cur_event->udata;
		cur_event->udata = NULL;
		disconnectFd(cur_event);
		return ;
	}
	else{
		// buff_[rlen] = '\0';
		// std::cout <<"들어온 버퍼::" << buff_ <<"|"<< std::endl;
		// std::cout << "그래서 저장된 raw_data:"<<std::endl;
		raw_data_ref.insert(raw_data_ref.end(), buff_, buff_ + rlen);
		// print_vec(raw_data_ref) ;
		std::vector<HttpRequest>& request_ref = cur_udata->http_request_;
		if (request_ref.size() && (request_ref.back().getParseStatus() != FINISH && !request_ref.back().getRequestError()))
			request_ref.back().parse(raw_data_ref);
		while (request_ref.size() == 0 || (request_ref.back().getParseStatus() == FINISH && !request_ref.back().getRequestError())) {
			request_ref.push_back(HttpRequest());
			request_ref.back().setPort(cur_udata->port_);
			request_ref.back().parse(raw_data_ref);
		}
		if (request_ref.back().getRequestError() != OK){//에러페이지 설정 때문에 앞으로 뺐습니다.
			cur_udata->http_response_ = HttpResponse(request_ref[0]);
		}
		switch (request_ref.back().getRequestError()){
			case (OK) : 
				break;
			case (FORM_ERROR) :
				return HttpResponseHandler::getInstance().errorCallBack(*cur_udata, 404);
			case (METHOD_ERROR) :
				return HttpResponseHandler::getInstance().errorCallBack(*cur_udata, 405);
			case (VERSION_ERROR) :
				return HttpResponseHandler::getInstance().errorCallBack(*cur_udata, 505);
			case (UNIMPLEMENTED_ERROR) :
				return HttpResponseHandler::getInstance().errorCallBack(*cur_udata, 501);
			case (LENGTH_REQUIRED_ERROR) :
				return HttpResponseHandler::getInstance().errorCallBack(*cur_udata, 411);
		}
		if (request_ref.size() != 0 && request_ref.front().getParseStatus() == FINISH)
			HttpResponseHandler::getInstance().parseResponse(cur_udata);
	}
}

/**
 * @brief 클라이언트 소켓이 writable할 때 호출되는 함수입니다.
 * @param cur_event 클라이언트 소켓에 해당되는 발생한 이벤트 구조체
 */
void  EventHandler::sockWritable(struct kevent *cur_event){
	std::cout << "SOCK Writable" << std::endl;
	UData*	cur_udata = (UData*)cur_event->udata;
	if (cur_udata == NULL){
		std::cout << cur_event->ident << "is already disconnected!(Write)"<< std::endl;
		return ;
	}

	std::vector<char>&	head_ref = cur_udata->http_response_.getJoinedData();
	std::vector<char>&	body_ref = cur_udata->http_response_.getBody();
	if (head_ref.size()) //여기가 첫번째 요청을 보내는 곳
		writeToclient(head_ref, false, cur_udata);
	else  //두번째 body를 보내는 분기입니다.
		writeToclient(body_ref, true, cur_udata);
}


/**
 * @brief cgi 파이프가 readable할 때 호출되는 함수입니다.
 * @param cur_event cgi 파이프에 해당되는 발생한 이벤트 구조체
 * @exception read()에서 에러 발생 시 runtime_error를 throw합니다.
 */
void  EventHandler::cgiReadable(struct kevent *cur_event){
	UData*	cur_udata = (UData*)cur_event->udata;
	std::vector<char>&	joined_data_ref = cur_udata->http_response_.joined_data_;
	int rlen = read(cur_event->ident, buff_, BUFF_SIZE);
	if (rlen == -1)
		throw(std::runtime_error("READ() ERROR!! IN CLNT_SOCK"));
	else if (rlen == 0){
		std::cout << "CGI process sent eof.\n";
    cur_udata->fd_type_ = CLNT;
    // disconnectFd(cur_event);
    waitpid(cur_udata->cgi_pid_, NULL, 0);
		close(cur_event->ident);
		cur_udata->http_response_.makeCgiResponse();
		Kqueue::registerWriteEvent(cur_udata->client_fd_, cur_udata);
  }
	else{
    // std::cout << "\nCGI HAS BEEN READ - SIZE: " << rlen << std::endl;
    // std::cout << "BUFFER:" << buff_ << "\n-------------------\n" << std::endl;
		// std::cout << "CGI의 버퍼다!"<< buff_ <<std::endl;
		joined_data_ref.insert(joined_data_ref.end(), buff_, buff_ + rlen);
    // std::string raw_data_string = std::string(raw_data_ref.begin(), raw_data_ref.end());
    // std::cout << "FROM CGI PROC" << raw_data_string << "\n";
    // Kqueue::unregisterReadEvent(cur_udata->client_fd_, cur_udata);//클라이언트 Read이벤트 잠시 중단
	}
}


void  EventHandler::cgiWritable(struct kevent *cur_event){
	std::cout << "CGI Writable" << std::endl;
	UData*	cur_udata = (UData*)cur_event->udata;
	const std::vector<char> &write_store_ref = cur_udata->http_request_[0].getBody();
	print_vec(write_store_ref);
	int write_size = write(cur_event->ident, &write_store_ref[cur_udata->write_size_], write_store_ref.size() - cur_udata->write_size_);
	if (write_size == -1) //실패하면 코드가 이상하긴 하다.
		return fileErrorCallBack(cur_event);
	cur_udata->write_size_+= write_size;
	if ((size_t)cur_udata->write_size_ == write_store_ref.size()){
    // std::cout << "여기 오냐?!!" << std::endl;
		close(cur_event->ident); //unregister?
		Kqueue::registerReadEvent(cur_udata->r_pfd, cur_udata);
		cur_udata->write_size_ = 0;
	}
}

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


/**
 * @brief 파일을 Read하는 이벤트가 발생했을 때 해당하는 파일을 Read합니다.
 * @param cur_event 해당하는 이벤트에 해당하는 Udata가 들어있는 cur_event
 */
void  EventHandler::fileReadable(struct kevent *cur_event){
	std::cout << "FILE READable" << std::endl;
	ssize_t read_len = read(cur_event->ident, buff_, BUFF_SIZE);
	UData*	cur_udata = (UData*)cur_event->udata;
	std::vector<char>& file_store_ref = cur_udata->http_response_.getBody();
	if (read_len == -1 || cur_udata->http_response_.file_size_ < static_cast<long>(read_len))//읽고 있는 파일을 삭제하는 경우 또는 파일크기보다 갑자기 더 큰게 읽히면 에러로 처리
		return fileErrorCallBack(cur_event);
	// buff_[read_len] = '\0';
	file_store_ref.insert(file_store_ref.end(), buff_, buff_ + read_len);
	// print_vec(file_store_ref);
	cur_udata->http_response_.file_size_ -= read_len;
	if (cur_udata->http_response_.file_size_ == 0){
		std::cout << "파일 다읽었어요~" <<std::endl;
		close(cur_event->ident);
		cur_udata->fd_type_= CLNT;
		cur_udata->http_response_.makeBodyResponse(cur_udata->http_response_.status_code_, file_store_ref.size());
		Kqueue::registerWriteEvent(cur_udata->client_fd_, cur_udata);
	}
}


/**
 * @brief 파일에 Write하는 이벤트가 발생했을 때 해당하는 파일에 write합니다.
 * @note Post에서 사용할 예정입니다.
 * @param cur_event 해당하는 이벤트에 해당하는 Udata가 들어있는 cur_event
 */
void	EventHandler::fileWritable(struct kevent *cur_event){//TODO: Max_body_size 어디서 처리할지 정하기.
	std::cout << "FILE Writable" << std::endl;
	UData*	cur_udata = (UData*)cur_event->udata;
	const std::vector<char> &write_store_ref = cur_udata->http_request_[0].getBody();
	int write_size = write(cur_event->ident, &write_store_ref[cur_udata->write_size_], write_store_ref.size() - cur_udata->write_size_);
	if (write_size == -1) //실패하면 코드가 이상하긴 하다.
		return fileErrorCallBack(cur_event);
	cur_udata->write_size_+= write_size;
	if ((size_t)cur_udata->write_size_ == write_store_ref.size()){
		cur_udata->http_response_.makeBodyResponse(201, 0);
		cur_udata->fd_type_ = CLNT;
		close(cur_event->ident);
		Kqueue::registerWriteEvent(cur_udata->client_fd_, cur_udata);
		cur_udata->write_size_ = 0;
	}
}


/**
 * @brief fd 연결을 끊는 함수
 * @param cur_event 해당되는 이벤트 구조체
 */
void  EventHandler::disconnectFd(struct kevent *cur_event){
	UData*	udata = (UData*)cur_event->udata;
	if (udata){
	  if (udata->fd_type_ == CLNT)
		  std::cout << "CLIENT DISCONNECTED: " << cur_event->ident << std::endl;
	  else if (udata->fd_type_ == CGI)
		  std::cout << "CGI FD DISCONNECTED: " << cur_event->ident << std::endl;
	}
	close(cur_event->ident);
}


void	EventHandler::writeToclient(std::vector<char> &to_write, bool is_body, UData*	cur_udata){
	int n;
	int w_size = cur_udata->write_size_;
	if (to_write.size() < (size_t) w_size){
		std::cout << "말이 안돼!" <<std::endl;
		return ;
	}
	n = write(cur_udata->client_fd_, &to_write[w_size], to_write.size() - w_size);
	cur_udata->write_size_ += n;
	if (n == -1){
		cur_udata->write_size_ = 0;
		to_write.clear();
		Kqueue::unregisterWriteEvent(cur_udata->client_fd_, cur_udata);
		HttpResponseHandler::getInstance().errorCallBack(*cur_udata, 500);
		return ;
	}
	else if ((size_t)cur_udata->write_size_ == to_write.size() || to_write.size() == 0){
		cur_udata->write_size_ = 0;
		if (is_body != true){
			std::cout << "헤더 보냈어요" <<std::endl;
			to_write.clear();
		}
		else{
			std::cout << "바디 보냈어요" <<std::endl;
			std::cout << "-----------------------" <<std::endl<<std::endl;
			Kqueue::unregisterWriteEvent(cur_udata->client_fd_, cur_udata);
			cur_udata->http_request_.erase(cur_udata->http_request_.begin());
			if (cur_udata->http_request_.size() != 0 && cur_udata->http_request_[0].getParseStatus() == FINISH)
				HttpResponseHandler::getInstance().parseResponse(cur_udata);
			else
				Kqueue::registerReadEvent(cur_udata->client_fd_, cur_udata);
		}
	}
}

void	EventHandler::fileErrorCallBack(struct kevent *cur_event){
	close(cur_event->ident);
	HttpResponseHandler::getInstance().errorCallBack(*(UData *)(cur_event->udata), 500);
}

EventHandler::EventHandler(){}

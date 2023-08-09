
# include "EventHandler.hpp"
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
	std::cout << "SOCK Readable" << std::endl;
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
	if (rlen == -1)
		std::cerr << "already disconnected!"<< std::endl;
	else if (rlen == 0){
		std::cout << "clnt sent eof. disconnecting." <<std::endl;
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
			http_request_ref.back().setPort(cur_udata->port_);
			http_request_ref.back().parse(raw_data_ref);
		}
		if (http_request_ref.back().getRequestError())
			 return HttpResponseHandler::getInstance().errorCallBack(*cur_udata, 500);
		if (http_request_ref.size() != 0)
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

	std::vector<char>&	first_response_ref = cur_udata->http_response_.getJoinedData();
	std::vector<char>&	second_response_ref = cur_udata->http_response_.getBody();
	int n;
	if (first_response_ref.size()){//여기가 첫번째 요청을 보내는 곳
		n = write(cur_event->ident, &first_response_ref[cur_udata->write_size_], first_response_ref.size() - cur_udata->write_size_);
		cur_udata->write_size_ += n;
		if (n == -1){
			cur_udata->http_response_.setStatusCode(500);//this is error
			cur_udata->write_size_ = 0;
			return ;
		}
		else if ((size_t)cur_udata->write_size_ == first_response_ref.size()){	
			first_response_ref.clear();
			cur_udata->write_size_ = 0;
		}
	}else { //두번째 body를 보내는 분기입니다.
		n = write(cur_event->ident, &second_response_ref[cur_udata->write_size_], second_response_ref.size() - cur_udata->write_size_);
			cur_udata->write_size_ += n;
		if (n == -1){
			std::cerr << "client write error!" << std::endl;
			cur_udata->http_response_.setStatusCode(500);//this is error
			// disconnectFd(cur_event);
			return ;
		}
		else if ((size_t)cur_udata->write_size_ == second_response_ref.size() || second_response_ref.size() == 0){	
			std::cout << "BODY END" << std::endl;
			std::cout << "--------------BODY size::"<<second_response_ref.size() <<std::endl;
			Kqueue::unregisterWriteEvent(cur_event->ident, cur_udata);
			cur_udata->http_request_.erase(cur_udata->http_request_.begin());
		}
		if (cur_udata->http_request_.size() != 0) //TODO:  클라이언트 ReadEvent를 Unregister하는게 두번째의 경우는 이미 unregister되어 있기 때문에 같은 함수를 호출하면 이미 안 된 이벤트를 다시 unregister 한다는게 이상하다.
			HttpResponseHandler::getInstance().parseResponse(cur_udata);
			//라고 생각했었는데, 이미 disable된 이벤트를 다시 disable해도 문제가 없다는 것을 보고 상관없음을 알았다. 확인은 필요할듯?
		else 
				Kqueue::registerReadEvent(cur_event->ident, cur_udata);
		cur_udata->write_size_ = 0;
	}
}


/**
 * @brief cgi 파이프가 readable할 때 호출되는 함수입니다.
 * @param cur_event cgi 파이프에 해당되는 발생한 이벤트 구조체
 * @exception read()에서 에러 발생 시 runtime_error를 throw합니다.
 */
void  EventHandler::cgiReadable(struct kevent *cur_event){
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
	}
}


/**
 * @brief CGI 프로세스를 회수하는 함수입니다.
 * @param udata pid가 담긴 udata입니다. 이후 CLNT용으로 전환됩니다.
 * @exception 자식이 비정상적으로 종료된 것이 감지되면 runtime_error를 throw합니다.
 */
void  EventHandler::cgiTerminated(UData* udata){
  int status;
  Kqueue::unregisterExitEvent(udata->cgi_pid_, udata);
  Kqueue::registerReadEvent(udata->client_fd_, udata);
  //TODO: 자식 프로세스의 fd[0]을 unregister해야할까요? 어차피 종료된 프로세스인데.. 그렇게 한다면 udata에 자식 프로세스 fd도 들고있어야 합니다.
  waitpid(udata->cgi_pid_, &status, 0);
  udata->fd_type_ = CLNT;
  udata->prog_name_ = "";
  udata->cgi_pid_ = 0;
  if (WIFEXITED(status))
    return;
  else
    throw std::runtime_error("CGI terminated abnormally");
}


/**
 * @brief 파일을 Read하는 이벤트가 발생했을 때 해당하는 파일을 Read합니다.
 * @param cur_event 해당하는 이벤트에 해당하는 Udata가 들어있는 cur_event
 */
void  EventHandler::fileReadable(struct kevent *cur_event){
	std::cout << "FILE READable" << std::endl;
	ssize_t read_len = read(cur_event->ident, buff_, BUFF_SIZE);
	UData*	cur_udata = (UData*)cur_event->udata;
	std::vector<char>& file_store_ref = cur_udata->http_response_.getBody();
	buff_[read_len] = '\0';
	file_store_ref.insert(file_store_ref.end(), buff_, buff_ + read_len);
	if (cur_udata->http_response_.file_size_ < static_cast<long>(read_len))
		return HttpResponseHandler::getInstance().errorCallBack(*cur_udata, 500);
	cur_udata->http_response_.file_size_ -= read_len;
	if (cur_udata->http_response_.file_size_ == 0){
		close(cur_event->ident);
		cur_udata->fd_type_= CLNT;
		if (read_len == -1)
			return HttpResponseHandler::getInstance().errorCallBack(*cur_udata, 500);
		cur_udata->http_response_.makeBodyResponse(200, file_store_ref.size());
		Kqueue::registerWriteEvent(cur_udata->client_fd_, cur_udata);
	}
} 


/**
 * @brief 파일에 Write하는 이벤트가 발생했을 때 해당하는 파일에 write합니다. 
 * @note Post에서 사용할 예정입니다.
 * @param cur_event 해당하는 이벤트에 해당하는 Udata가 들어있는 cur_event
 */
void	EventHandler::fileWritable(struct kevent *cur_event){
	UData*	cur_udata = (UData*)cur_event->udata;
	const std::vector<char> &write_store_ref = cur_udata->http_request_[0].getBody();
	int write_size = write(cur_event->ident, &write_store_ref[cur_udata->write_size_], write_store_ref.size() - cur_udata->write_size_);
	cur_udata->write_size_+= write_size;
	if ((size_t)cur_udata->write_size_ == write_store_ref.size()){
		cur_udata->http_response_.makeNoBodyResponse(201);
		cur_udata->fd_type_ = CLNT;
		Kqueue::unregisterWriteEvent(cur_event->ident, cur_udata);
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
  if (udata->fd_type_ == CLNT)
	  std::cout << "CLIENT DISCONNECTED: " << cur_event->ident << std::endl;
  else if (udata->fd_type_ == CGI)
	  std::cout << "CGI PROCESS TERMINATED: " << udata->cgi_pid_ << std::endl;
	close(cur_event->ident);
	delete udata;
	cur_event->udata = NULL;
}

EventHandler::~EventHandler(){}
EventHandler::EventHandler(){}

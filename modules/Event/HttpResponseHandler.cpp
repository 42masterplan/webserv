# include "HttpResponseHandler.hpp"

HttpResponseHandler& HttpResponseHandler::getInstance(){
	static HttpResponseHandler parser;
	return parser;
}

void	HttpResponseHandler::parseResponse(UData *udata){
	std::cout << "PARSE RESPONSE!!!!!!" << std::endl;
	HttpRequest &cur_request = udata->http_request_[0];
	udata->http_response_ = HttpResponse(cur_request);
	handleResponse(udata);
}


void	HttpResponseHandler::handleResponse(UData *udata){
	HttpResponse &cur_response = udata->http_response_;
	// HttpRequest &cur_request = udata->http_request_[0];
	std::cout << "RESTYPE: " << static_cast<int>(cur_response.res_type_) << std::endl;
	switch(cur_response.res_type_){
		case METHOD_TYPE : handleHttpMethod(*udata);
			break ;
		case CGI_EXEC : Cgi::forkCgi(udata);
			break ;
		case AUTOINDEX : //TODO: 이거 이벤트 어디서 등록할까요?
		  if(isDenyMethod(*udata, udata->http_request_[0].getMethod()))
			  return errorCallBack(*udata, 405);
			cur_response.body_ = AutoIndex::getDirectoryListing(cur_response.getFilePath().c_str());
      cur_response.makeBodyResponse(200, cur_response.body_.size());
			if (cur_response.body_.size() != 0)
      	RegisterClientWriteEvent(*udata);
			else 
				errorCallBack(*udata, 404);
			break ;
		case REDIRECT : 
		cur_response.processRedirectRes(cur_response.loc_block_.getReturnCode());//여기서 첫번째 줄과 헤더 합쳐서 메세지 다 만들어서 joined_data_에 넣어줍니다. 
			RegisterClientWriteEvent(*udata);
			break ;
		case ERROR : 
			if(isDenyMethod(*udata, udata->http_request_[0].getMethod()))
			  return errorCallBack(*udata, 405);
			errorCallBack(*udata, udata->http_response_.status_code_);
		//에러코드를 확인해서 해당하는 status code에 해당하는 에러페이지가 있는지 탐색-> 있다면 그 파일을 write 이벤트 등록 아니라면 default error_page 만들어서 클라이언트에게 write
			break ;
	}
}

std::string HttpResponseHandler::convertToStr(e_method method) {
	switch(method) {
		case GET:
			return "GET";
		case HEAD:
			return "HEAD";
		case DELETE:
			return "DELETE";
		case POST:
			return "POST";
		case PUT:
			return "PUT";
		case PATCH:
			return "PATCH";
		default:
			return "";
	}
}

/**
 * @brief deny_method vector가 채워져 있으면 그 메서드만 사용이 가능한 method입니다.
 * 따라서 default.conf파일을 볼 때 GET요청에 대해서는 auto index를 반환해야만 합니다.!
 * @param udata 
 * @param method 
 * @return true 
 * @return false 
 */
bool HttpResponseHandler::isDenyMethod(UData &udata, e_method method) {
	const std::vector<std::string> deny_method = udata.http_response_.loc_block_.getDenyMethod();
	if (deny_method.size() == 0)
		return false;
	if (std::find(deny_method.begin(), deny_method.end(), convertToStr(method)) != deny_method.end())//메서드 deny
		return false;
	return true;
}

void HttpResponseHandler::handleHttpMethod(UData &udata) {
	const e_method method = udata.http_request_[0].getMethod();
	std::cout << "handle METHOD!!!!" << std::endl;
	if(isDenyMethod(udata, method))
		return errorCallBack(udata, 405);
	switch(method) {
		case GET:
			return handleGet(udata);
		case HEAD:
			return handleHead(udata);
		case DELETE:
			return handleDelete(udata);
		case POST:
			return handlePost(udata);
		case PUT:
			return handlePost(udata);
		case PATCH:
			return handlePost(udata);
		default:
			return ;
	}
}

void HttpResponseHandler::handleGet(UData &udata) {
	std::cout << "GET!" << udata.http_response_.getFilePath().c_str() << std::endl;
	int fd = open(udata.http_response_.getFilePath().c_str(), O_RDONLY);
	if (fd == -1)
		return errorCallBack(udata, 404);
	udata.http_response_.setFileSize(udata.http_response_.getFilePath());
	RegisterFileReadEvent(fd, udata);
}

void HttpResponseHandler::handleHead(UData &udata) {
	std::cout << "HEAD!!" << udata.http_response_.getFilePath().c_str() << std::endl;
	int fd = open(udata.http_response_.getFilePath().c_str(), O_RDONLY);
	if (fd == -1)
		udata.http_response_.processErrorRes(404);
	udata.http_response_.setFileSize(udata.http_response_.getFilePath());
	RegisterClientWriteEvent(udata);
}

void HttpResponseHandler::handlePost(UData &udata) {
	std::cout << "POST!!!!!!!" << std::endl;
	std::string filename = udata.http_response_.getFilePath() + MimeStore::getExtension(udata.http_request_[0].getContentType());
	int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return errorCallBack(udata, 500);
	udata.http_response_.setFileSize(udata.http_response_.getFilePath());
	RegisterFileWriteEvent(fd, udata);
}

void HttpResponseHandler::handleDelete(UData &udata) {
	std::remove(udata.http_response_.getFilePath().c_str());
	udata.http_response_.makeNoBodyResponse(200);
	//삭제에 성공하던 실패하던 같은 코드를 내보낸다.
	//TODO:delete 성공했을 때 header와 body를 만들어서 메세지를 만든 후에 아래 이벤트를 등록한다.
	RegisterClientWriteEvent(udata);
}

/**
 * @brief 에러 코드 설정 및 파일 위치 설정 합니다.
 * 
 * @param udata 해당 udata
 * @param status_code 설정하고 싶은 status code
 */
void	HttpResponseHandler::errorCallBack(UData &udata, int status_code){
	std::cout << "\n\nSTATCODE: " << status_code << "\n\n";
	udata.http_response_.processErrorRes(status_code);
	int error_file_fd_ ;
	std::cout << "ERROR file PATH " <<udata.http_response_.getFilePath() <<std::endl;
	if (udata.http_request_[0].getMethod() == HEAD){
			udata.http_response_.processErrorRes(status_code);
			udata.http_response_.setFileSize(udata.http_response_.getFilePath());
			udata.http_response_.makeBodyResponse(status_code, udata.http_response_.file_size_);
			RegisterClientWriteEvent(udata);
	}
	else if (udata.http_response_.getFilePath() != ""){
		error_file_fd_ = open(udata.http_response_.getFilePath().c_str(), O_RDONLY);
		if (error_file_fd_ == -1) //TODO: 500인데 그 에러파일 위치가 없다면? 처리해야함
			return errorCallBack(udata, 500);
		udata.http_response_.setFileSize(udata.http_response_.getFilePath());
		RegisterFileReadEvent(error_file_fd_, udata);
	}
	//에러페이지가 설정되지 않는 경우가 존재하려나?
	else 
		std::cout << "NO ERORRPAGE!!" << std::endl;
}

void	HttpResponseHandler::RegisterClientWriteEvent(UData &udata){
	udata.fd_type_ = CLNT;
	Kqueue::registerWriteEvent(udata.client_fd_, &udata);//파일 write 이벤트 등록
	Kqueue::unregisterReadEvent(udata.client_fd_, &udata);//클라이언트 Read이벤트 잠시 중단
}

void	HttpResponseHandler::RegisterFileWriteEvent(int file_fd ,UData &udata){
  fcntl(file_fd, F_SETFL, O_NONBLOCK);
  udata.fd_type_ = FILETYPE;
  Kqueue::registerWriteEvent(file_fd, &udata);//파일 write 이벤트 등록
  Kqueue::unregisterReadEvent(udata.client_fd_, &udata);//클라이언트 Read이벤트 잠시 중단
}

void	HttpResponseHandler::RegisterFileReadEvent(int file_fd ,UData &udata){
	fcntl(file_fd, F_SETFL, O_NONBLOCK);
	udata.fd_type_ = FILETYPE;
	Kqueue::registerReadEvent(file_fd, &udata);//파일 ReadEvent 등록
	Kqueue::unregisterReadEvent(udata.client_fd_, &udata);//클라이언트 Read이벤트 잠시 중단
}

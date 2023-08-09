# include "HttpResponseHandler.hpp"

HttpResponseHandler& HttpResponseHandler::getInstance(){
	static HttpResponseHandler parser;
	return parser;
}

void	HttpResponseHandler::parseResponse(UData *udata){
	HttpRequest &cur_request = udata->http_request_[0];
	udata->http_response_ = HttpResponse(cur_request);
	//아래 switch case에서 해당하는 이벤트 등록
	std::cout << udata->http_response_.loc_block_.getCombineLocPath() <<std::endl;

	std::cout << udata->http_response_.res_type_ << std::endl;
	handleResponse(udata);
}


void	HttpResponseHandler::handleResponse(UData *udata){
	HttpResponse &cur_response = udata->http_response_;
	HttpRequest &cur_request = udata->http_request_[0];
		
	switch(cur_response.res_type_){
		case METHOD_TYPE : handleHttpMethod(*udata);
			break ;
		case CGI_EXEC : Cgi::forkCgi(cur_request); //CGI이벤트 등록 
    /*
    1. 호출 클라이언트 udata 인자로 받아서 fd_type CGI로 변경 후 사용
    2. CGI종료 시 다시 udata fd_type 클라이언트로 변경하며 CGI사용 자원 초기화
    */
		//TODO: 클라이언트 fd를 알고 있어야해요!! CGI 처리를 끝내고 그곳에서 클라이언트 이벤트를 등록할 꺼라서 개인적으로 같은 UDATA 새로 안 만들고 같은 Udata 써도 문제 없을 것 같아보여요
			break ;
		case AUTOINDEX : //TODO: 이거 이벤트 어디서 등록할까요?
		  if(isDenyMethod(*udata, udata->http_request_[0].getMethod()))
			  return errorCallBack(*udata, 405);
			cur_response.body_ = AutoIndex::getDirectoryListing(cur_response.getFilePath().c_str());
			break ;
		case REDIRECT : RegisterClientWriteEvent(*udata);
			break ;
		case ERROR : errorCallBack(*udata, udata->http_response_.status_code_);
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

bool HttpResponseHandler::isDenyMethod(UData &udata, e_method method) {
	const std::vector<std::string> deny_method = udata.http_response_.loc_block_.getDenyMethod();
	if (std::find(deny_method.begin(), deny_method.end(), convertToStr(method)) == deny_method.end())//메서드 deny
		return true;
	return false;
}

void HttpResponseHandler::handleHttpMethod(UData &udata) {
	const e_method method = udata.http_request_[0].getMethod();
	if(isDenyMethod(udata, method))
		return errorCallBack(udata, 405);

	std::cout << "HI" <<std::endl;
	switch(method) {
		case GET:
			return handleHeadGet(udata);
		case HEAD:
			return handleHeadGet(udata);
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

void HttpResponseHandler::handleHeadGet(UData &udata) {
	std::cout << "GET!! OR HEAD" << udata.http_response_.getFilePath().c_str() << std::endl;
	int fd = open(udata.http_response_.getFilePath().c_str(), O_RDONLY);
	if (fd == -1)
		return errorCallBack(udata, 404);
	RegisterFileReadEvent(fd, udata);
}

void HttpResponseHandler::handlePost(UData &udata) {
	std::string filename = udata.http_response_.getFilePath() + MimeStore::getExtension(udata.http_request_[0].getContentType());
	int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return errorCallBack(udata, 500);
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
	std::cout << "statusCode :" << status_code << std::endl;
	udata.http_response_.processErrorRes(status_code);
	int error_file_fd_ ;
	if (udata.http_response_.getFilePath() != ""){
		error_file_fd_ = open(udata.http_response_.getFilePath().c_str(), O_RDONLY);
		if (status_code != 500 && error_file_fd_ == -1) //TODO: 500인데 그 에러파일 위치가 없다면? 처리해야함
			return errorCallBack(udata, 500);
		RegisterFileWriteEvent(error_file_fd_, udata);
	}
	//에러페이지기 설정되지 않는 경우가 존재하려나?
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

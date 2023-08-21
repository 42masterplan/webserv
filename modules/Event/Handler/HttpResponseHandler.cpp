# include "HttpResponseHandler.hpp"

HttpResponseHandler& HttpResponseHandler::getInstance(){
	static HttpResponseHandler parser;
	return parser;
}

void	HttpResponseHandler::parseResponse(UData *udata){
	HttpRequest &req = udata->http_request_[0];
	udata->http_response_ = HttpResponse(req);
	handleResponse(udata);
}

void	HttpResponseHandler::handleResponse(UData *udata){
	HttpResponse &res = udata->http_response_;
	if (res.loc_block_.isErrorBlock() == true)
		return errorCallBack(*udata, 404);
	switch(res.res_type_){
		case METHOD_TYPE : handleHttpMethod(*udata);
			break ;
		case CGI_EXEC : Cgi::forkCgi(udata);
			break ;
		case AUTOINDEX :
		  if(isDenyMethod(*udata, udata->http_request_[0].getMethod()))
			  return errorCallBack(*udata, 405);
			res.body_ = AutoIndex::getDirectoryListing(res);
      res.setStatusCode(200);
			res.setContentLength(res.body_.size());
      res.makeBodyResponse();
			if (res.body_.size() != 0)
      	RegisterClientWriteEvent(*udata);
			else 
				errorCallBack(*udata, 404);
			break ;
		case REDIRECT : 
			res.setStatusCode(res.loc_block_.getReturnCode());//여기서 첫번째 줄과 헤더 합쳐서 메세지 다 만들어서 joined_data_에 넣어줍니다. 
			res.setStatusCode(res.loc_block_.getReturnCode());
			res.setContentLength(0);
			res.makeBodyResponse();//여기서 첫번째 줄과 헤더 합쳐서 메세지 다 만들어서 joined_data_에 넣어줍니다. 
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
	std::cout << CYAN << "[GET]" << CLOSE << std::endl;
	if (isFolder(udata.http_response_.getFilePath().c_str()) == true)
		return errorCallBack(udata, 404);
	int fd = open(udata.http_response_.getFilePath().c_str(), O_RDONLY);
	if (fd == -1)
		return errorCallBack(udata, 404);
	udata.http_response_.setFileSize(udata.http_response_.getFilePath());
	if (udata.http_response_.file_size_ == 0){//파일이 있는데 크기가 0인경우
		close(fd);
		udata.http_response_.setStatusCode(200);
		udata.http_response_.setContentLength(0);
		udata.http_response_.makeBodyResponse();
		RegisterClientWriteEvent(udata);
	}
	else
		RegisterFileReadEvent(fd, udata);
}

void HttpResponseHandler::handleHead(UData &udata) {
	std::cout << CYAN << "[HEAD] " << udata.http_response_.getFilePath() << CLOSE << std::endl;
	int fd = open(udata.http_response_.getFilePath().c_str(), O_RDONLY);
	if (fd == -1)
		udata.http_response_.processErrorRes(404);
	udata.http_response_.setFileSize(udata.http_response_.getFilePath());
	RegisterClientWriteEvent(udata);
}

void HttpResponseHandler::handlePost(UData &udata) {
	std::string filename = udata.http_response_.getFilePath() + MimeStore::getExtension(udata.http_request_[0].getContentType());
	std::cout << CYAN << "[POST/PUT/PATCH]" << CLOSE << std::endl;
	std::cout << CYAN << "저장 파일 경로: " << filename << CLOSE << std::endl;
	
	udata.http_response_.setLocation(filename);
	size_t max_body_size = udata.http_response_.loc_block_.getClientMaxBodySize();
	if (max_body_size!= 0 && udata.http_request_[0].getBody().size() > max_body_size )
		return errorCallBack(udata, 413);
	int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return errorCallBack(udata, 500);
	RegisterFileWriteEvent(fd, udata);
}

void HttpResponseHandler::handleDelete(UData &udata) {
	std::string filename = udata.http_response_.getFilePath();

	//삭제에 성공하던 실패하던 같은 코드를 내보낸다.
	remove(filename.c_str());
	std::string result =
	"<!DOCTYPE html>\n"
	"<html>\n"
	"  <body>\n"
	"    <h1>File deleted.</h1>\n"
	"  </body>\n"
	"</html>";

	udata.http_response_.body_ = std::vector<char>(result.begin(), result.end());
	udata.http_response_.setStatusCode(200);
	udata.http_response_.setContentLength(result.size());
	udata.http_response_.makeBodyResponse();
	RegisterClientWriteEvent(udata);
}

/**
 * @brief 에러 코드 설정 및 파일 위치 설정 합니다.
 * 
 * @param udata 해당 udata
 * @param status_code 설정하고 싶은 status code
 */
void	HttpResponseHandler::errorCallBack(UData &udata, int status_code){
	std::cout << YELLOW << "ERROR STATUS CODE: " << status_code << CLOSE << std::endl;
	udata.http_response_.processErrorRes(status_code);
	int error_file_fd_ ;
	if (udata.http_response_.getFilePath() == "" || status_code == 500){
		error_file_fd_ = open(udata.http_response_.getFilePath().c_str(), O_RDONLY);
		if (error_file_fd_ != -1){
			udata.http_response_.setFileSize(udata.http_response_.getFilePath());
			RegisterFileReadEvent(error_file_fd_, udata);
			return ;
		}
		else{
			udata.http_response_.file_size_ = 0;
			udata.http_response_.setStatusCode(status_code);
			udata.http_response_.setContentLength(0);
			udata.http_response_.makeBodyResponse();
			RegisterClientWriteEvent(udata);
			return ;
		}
	}
	if (udata.http_request_[0].getMethod() == HEAD){
			udata.http_response_.processErrorRes(status_code);
			udata.http_response_.setFileSize(udata.http_response_.getFilePath());
			udata.http_response_.setStatusCode(status_code);
			udata.http_response_.setContentLength(udata.http_response_.file_size_);
			udata.http_response_.makeBodyResponse();
			RegisterClientWriteEvent(udata);
	}
	else{
		error_file_fd_ = open(udata.http_response_.getFilePath().c_str(), O_RDONLY);
		if (error_file_fd_ == -1)
			return errorCallBack(udata, 500);
		udata.http_response_.setFileSize(udata.http_response_.getFilePath());
		RegisterFileReadEvent(error_file_fd_, udata);
	}
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

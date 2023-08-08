# include "HttpResponseHandler.hpp"

HttpResponseHandler& HttpResponseHandler::getInstance(){
	static HttpResponseHandler parser;
	return parser;
}

void	HttpResponseHandler::parseResponse(UData *udata){
	HttpRequest &cur_request = udata->http_request_[0];
	udata->http_response_ = HttpResponse(cur_request);
	//아래 switch case에서 해당하는 이벤트 등록
	handleResponse(udata);
}



void	HttpResponseHandler::handleResponse(UData *udata){
	HttpResponse &cur_response = udata->http_response_;
	HttpRequest &cur_request = udata->http_request_[0];
	switch(cur_response.res_type_){
		case UPLOAD_STORE : handleHttpMethod(*udata);
			break ;
		case CGI_EXEC : Cgi::forkCgi(cur_request); //CGI이벤트 등록
			break ;
		case REDIRECT : //클라이언트에게 write이벤트 등록하기
			break ;
		case ERROR : //에러코드를 확인해서 해당하는 status code에 해당하는 에러페이지가 있는지 탐색-> 있다면 그 파일을 write 이벤트 등록 아니라면 default error_page 만들어서 클라이언트에게 write
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

void HttpResponseHandler::handleHttpMethod(UData &udata) {
	e_method method = udata.http_request_[0].getMethod();
	const std::vector<std::string> deny_method = udata.http_response_.loc_block_.getDenyMethod();

	// if (std::find(deny_method.begin(), deny_method.end(), convertToStr(method)) == deny_method.end())
		// return res.processErrorRes(405);
	switch(method) {
		case GET:
			return handleGet(udata);
		case HEAD:
			return handleGet(udata);
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
	// int fd = open(udata.http_response_.getFilePath().c_str(), O_RDONLY);
	// if (fd == -1)
	// 	return udata.http_response_.processErrorRes(404);
	// fcntl(fd, F_SETFL, O_NONBLOCK);
	// Kqueue::registerReadEvent(fd, &udata);
	// Kqueue::unregisterWriteEvent(fd, &udata);
	(void) udata;
}

void HttpResponseHandler::handleHead(UData &udata) {
	int fd = open(udata.http_response_.getFilePath().c_str(), O_RDONLY);
	if (fd == -1)
		return udata.http_response_.processErrorRes(404);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	Kqueue::registerReadEvent(fd, &udata);
	Kqueue::unregisterWriteEvent(fd, &udata);
}

void HttpResponseHandler::handlePost(UData &udata) {
	std::string filename = udata.http_response_.getFilePath() + MimeStore::getExtension(udata.http_request_[0].getContentType());
	int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return udata.http_response_.processErrorRes(404);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	Kqueue::registerWriteEvent(fd, &udata);
	Kqueue::unregisterReadEvent(fd, &udata);
}

void HttpResponseHandler::handleDelete(UData &udata) {
	std::remove(udata.http_response_.getFilePath().c_str());
}

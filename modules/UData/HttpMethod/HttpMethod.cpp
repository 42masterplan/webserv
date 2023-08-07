#include "HttpMethod.hpp"

static std::string convertToStr(e_method method) {
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

void HttpMethod::handleHttpMethod(UData &udata) {
	e_method method = udata.http_request_[0].getMethod();
	const std::vector<std::string> deny_method = udata.http_response_.getLocBlock().getDenyMethod();

	if (std::find(deny_method.begin(), deny_method.end(), convertToStr(method)) == deny_method.end())
		return res.processErrorRes(405);
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
			return res.processErrorRes(405);
	}
}

void HttpMethod::handleHead(UData &udata) {
	int fd = open(udata.http_response_.getFilePath().c_str(), O_RDONLY);
	if (fd == -1)
		return udata.http_response_.processErrorRes(404);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	Kqueue::registerReadEvent(fd, &udata);
	Kqueue::unregisterWriteEvent(fd, &udata);
}

void HttpMethod::handlePost(UData &udata) {
	std::string filename = udata.http_response_.getFilePath() + MimeStore::getExtension(udata.http_request_[0].getContentType());
	int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		return udata.http_response_.processErrorRes(404);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	Kqueue::registerWriteEvent(fd, &udata);
	Kqueue::unregisterReadEvent(fd, &udata);
}

void HttpMethod::handleDelete(UData &udata) {
	std::remove(udata.http_response_.getFilePath().c_str());
}


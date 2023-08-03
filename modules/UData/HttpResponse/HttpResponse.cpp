#include "HttpResponse.hpp"

std::map<int, std::string> HttpResponse::status_store_;

HttpResponse::HttpResponse() {}

void HttpResponse::initStatusStore(void) {
	status_store_[200] = "200 OK";
	status_store_[201] = "201 Created";
	status_store_[202] = "202 Accepted";
	status_store_[204] = "204 No Content";
	status_store_[301] = "301 Moved Permanently";
	status_store_[302] = "302 Found";
	status_store_[304] = "304 Not Modified";
	status_store_[400] = "400 Bad Request";
	status_store_[401] = "401 Unauthorized";
	status_store_[403] = "403 Forbidden";
	status_store_[404] = "404 Not Found";
	status_store_[405] = "405 Method Not Allowed";
	status_store_[500] = "500 Internal Server Error";
	status_store_[501] = "501 Not Implemented";
	status_store_[502] = "502 Bad Gateway";
	status_store_[503] = "503 Service Unavailable";
}

/**
 * @brief 반환할 ErrorLocation이 없는 경우에 사용
 
 * 
 * @param res 채워질 HttpResponse 객체
 * @param status_code 
 */
void HttpResponse::processDefaultErrorRes(HttpResponse &res, int status_code) {
	status_code_ = status_code;

	http_version_ = "HTTP/1.1";
	status_ = status_store_[status_code_];
	std::string body = 
	"<html><head><title>" + status_ + "</title></head><body><h1>" + status_ + "</h1></body></html>";
	body_.insert(body_.end(), body.begin(), body.end());

	content_type_ = "text/html";
	content_length_ = body.size();
	
	std::string header =
	http_version_ + " " + status_ + "\r\n" +
	"ContentType: " + content_type_ + "\r\n" +
	"ContentLength: " + std::to_string(content_length_) + "\r\n\r\n";
	
	joined_data_.insert(joined_data_.end(), header.begin(), header.end()); // insert 말고 덮어써야함
	joined_data_.insert(joined_data_.end(), body_.begin(), body_.end());
	std::cout << std::string(res.joined_data_.begin(), res.joined_data_.end()) << "\n"; 
}


void HttpResponse::processRedirectRes(HttpResponse &res, int status_code) {
	status_code_ = status_code;
	status_ = status_store_[status_code_];
	
	std::string header =
	http_version_ + " " + status_ + "\r\n" +
	"Location: " + location_ + "\r\n\r\n";
	
	joined_data_.insert(joined_data_.end(), header.begin(), header.end()); // insert 말고 덮어써야함
	std::cout << std::string(res.joined_data_.begin(), res.joined_data_.end()) << "\n";
}

int main() {
	HttpResponse res;
	res.initStatusStore();
	res.processDefaultErrorRes(res, 404);
	res.processRedirectRes(res, 301);
	return 0;
}
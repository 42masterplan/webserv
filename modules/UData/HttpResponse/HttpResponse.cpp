#include "HttpResponse.hpp"

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

const std::vector<char>& HttpResponse::getJoinedData()const{return joined_data_;}

int main() {
	HttpResponse res;
	res.initStatusStore();
	res.processDefaultErrorRes(res, 404);
	res.processRedirectRes(res, 301);
	return 0;
}
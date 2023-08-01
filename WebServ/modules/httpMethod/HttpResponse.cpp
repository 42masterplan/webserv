#include "HttpResponse.hpp"

std::map<int, std::string> HttpResponse::status_store_ = {
	{200, "200 OK"},
	{201, "201 Created"},
	{202, "202 Accepted"},
	{204, "204 No Content"},
	{301, "301 Moved Permanently"},
	{302, "302 Found"},
	{304, "304 Not Modified"},
	{400, "400 Bad Request"},
	{401, "401 Unauthorized"},
	{403, "403 Forbidden"},
	{404, "404 Not Found"},
	{405, "405 Method Not Allowed"},
	{500, "500 Internal Server Error"},
	{501, "501 Not Implemented"},
	{502, "502 Bad Gateway"},
	{503, "503 Service Unavailable"}
};

void HttpResponse::processDefaultErrorRes(HttpResponse &res, int status_code) {
	status_code_ = status_code;
	status_ = status_store_[status_code_];
	std::string body_ = 
	"<html>\
		<head><title>" + status_ + "</title></head>\
		<body><h1>" + status_ + "</h1></body>\
	</html>";
	
	content_type_ = "text/html";
	content_length_ = body_.size();
	
	std::string header =
	http_version_ + " " + status_ + "\r\n" +
	"ContentType: " + content_type_ + "\r\n" +
	"ContentLength: " + std::to_string(content_length_) + "\r\n\r\n";
	
	joined_data_.insert(joined_data_.end(), header.begin(), header.end());
	joined_data_.insert(joined_data_.end(), body_.begin(), body_.end());
}

void HttpResponse::processDefaultRedirectRes(HttpResponse &res, int status_code) {
	HttpResponse res = udata.http_response_;

	res.setStatusCode(status_code);
	res.setStatus(status_store_[status_code]);
	res.setLocation("redirect_location");
}

#include "HttpResponse.hpp"

HttpResponse::HttpResponse(){}
HttpResponse& HttpResponse::operator=(const HttpResponse &ref) {
	if (this == &ref)
		return *this;
	http_version_ = ref.http_version_;
	status_code_ = ref.status_code_;
	status_ = ref.status_;
	content_length_ = ref.content_length_;
	content_type_ = ref.content_type_;
	location_ = ref.location_;
	body_ = ref.body_;
	joined_data_ = ref.joined_data_;
	res_type_ = ref.res_type_;
	file_path_ = ref.file_path_;
	client_fd_ = ref.client_fd_;
	write_size_ = ref.write_size_;

	return *this;
}

HttpResponse::HttpResponse(HttpRequest &req) : http_version_("HTTP/1.1"),  status_code_(200), status_(""), content_length_(0), content_type_(""), location_(""), loc_block_((ConfParser::getInstance().getServBlock(req.getPort(), req.getHost())).findLocBlock(req.getPath())), res_type_(UPLOAD_STORE), file_path_("") {
	setFilePath(req, loc_block_);
}

/* init */

bool HttpResponse::isExistFile(std::string &filePath) {
	std::ifstream file(filePath.c_str());
	return file.good();
}


// static void handleHttpError() {
// 	if (res.getFilePath() == "")
// 		res.processDefaultErrorRes(res.getStatusCode());
// 	res.processDefaultErrorRes(res.getStatusCode());
// }

void HttpResponse::processErrorRes(int status_code) {
	status_code_ = status_code;
	res_type_ = ERROR;
	file_path_ = getErrorPagePath(status_code);

	
	// Kqueue::registerReadEvent(fd, &udata);//파일 ReadEvent 등록
	// Kqueue::unregisterReadEvent(udata.client_fd_, &udata);//클라이언트 Read이벤트 
}

void HttpResponse::processRedirectRes(int status_code) {
	status_code_ = status_code;
	status_ = status_store_[status_code_];
	
	std::string header =
	http_version_ + " " + status_ + "\r\n" +
	"Location: " + location_ + "\r\n\r\n";
	
	joined_data_.clear();
	joined_data_.insert(joined_data_.end(), header.begin(), header.end());
	std::cout << std::string(joined_data_.begin(), joined_data_.end()) << "\n";
}

std::string HttpResponse::getErrorPagePath(int status_code){
	std::vector<int> error_codes = loc_block_.getErrorCode();
	std::vector<int>::iterator it = std::find(error_codes.begin(), error_codes.end(), status_code);	
	if (it != error_codes.end())
		return loc_block_.getCombineErrorPath();
	return std::string("/var/www/errorPages/404.html");
}

/* getter, setter */

void HttpResponse::setStatusCode(int status_code) {
	status_code_ = status_code;
}

std::vector<char> &HttpResponse::getBody() {
	return body_;
}

const std::string &HttpResponse::getFilePath() const {
	return file_path_;
}

void HttpResponse::setFilePath(HttpRequest &req, LocBlock &loc) {
	file_path_ = loc.getCombineReturnPath();
	(void) req;
	if (file_path_ != "") {
		res_type_ = REDIRECT;
		location_ = file_path_;
		return;
	}
	//is autoindex
	file_path_ = loc.getCombineCgiPath();
	if (file_path_ != "") {
		res_type_ = CGI_EXEC;
		return;
	}
	file_path_ = loc.getCombineUploadStorePath();
	if (file_path_ != "") {
		res_type_ = UPLOAD_STORE;
		return;
	}
	processErrorRes(404);
	return;
}




const std::vector<char>& HttpResponse::getJoinedData() const {
	return joined_data_;
}


// int main() {
// 	HttpResponse res;
// 	res.initStatusStore();
// 	res.processDefaultErrorRes(res, 404);
// 	res.processRedirectRes(res, 301);
// 	return 0;
// }

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
	loc_block_ = ref.loc_block_;
	return *this;
}

HttpResponse::HttpResponse(HttpRequest &req) : http_version_("HTTP/1.1"),  status_code_(200), status_(""), content_length_(0), content_type_(""), location_(""), loc_block_((ConfParser::getInstance().getServBlock(req.getPort(), req.getHost())).findLocBlock(req.getPath())), res_type_(METHOD_TYPE), file_path_("") {
	setFilePath(req, loc_block_);
}

/* init */

bool HttpResponse::isExistFile(std::string &filePath) {
	std::ifstream file(filePath.c_str());
	return file.good();
}

void HttpResponse::setErrorCodePath(int status_code) {
	status_code_ = status_code;
	res_type_ = ERROR;
	file_path_ = getErrorPagePath(status_code);
}

void HttpResponse::processRedirectRes(int status_code) {
	status_code_ = status_code;
	status_ = status_store_[status_code_];
	
	std::string header =
	http_version_ + " " + status_ + "\r\n" +
	"Location: " + location_ + "\r\n\r\n";
	
	joined_data_.clear();
	joined_data_.insert(joined_data_.end(), header.begin(), header.end());
	// std::cout << std::string(joined_data_.begin(), joined_data_.end()) << "\n";
}

/**
 * @brief 에러가 나지 않고 POST에 성공했을 때 등에만 사용한다.보낼 body가 있을 대 사용해서는 안된다.
 * 
 * @param status_code 
 */
void	HttpResponse::makeNoBodyResponse(int status_code){
	status_code_ = status_code;
	status_ = status_store_[status_code_];
	std::string header =
	http_version_ + " " + status_ + "\r\n\r\n";
	joined_data_.clear();
	joined_data_.insert(joined_data_.end(), header.begin(), header.end());
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
		processRedirectRes(loc.getReturnCode());//여기서 첫번째 줄과 헤더 합쳐서 메세지 다 만들어서 joined_data_에 넣어줍니다.
		return;
	}
	//is autoindex
	file_path_ = loc.getCombineCgiPath();
	if (file_path_ != "") {
		res_type_ = CGI_EXEC;
		return;
	}
	file_path_ = loc.getCombineUploadStorePath();
	if (file_path_ == "" && 3 <= req.getMethod() && req.getMethod() <= 5) {//upload 하려고 하는데 그 경로가 설정파일에서 없으면 서버에러
		res_type_ = ERROR;
		setErrorCodePath(500);
		return;
	} else 
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

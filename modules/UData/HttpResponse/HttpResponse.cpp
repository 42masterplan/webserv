#include "HttpResponse.hpp"


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
	loc_block_ = ref.loc_block_;
	res_type_ = ref.res_type_;
	file_path_ = ref.file_path_;
	client_fd_ = ref.client_fd_;
	write_size_ = ref.write_size_;

	return *this;
}

HttpResponse::HttpResponse(UData &udata, HttpRequest &req) : http_version_("HTTP/1.1"),  status_code_(200), status_(""), content_length_(0), content_type_(""), location_(""), loc_block_(initLocBlock(req)), res_type_(UPLOAD_STORE), file_path_("") {
	setFilePath(req, loc_block_);
	
}

/* init */

static LocBlock &initLocBlock(HttpRequest &req) {
	ServBlock serv = ConfParser::getInstance().getServBlock(req.getPort(), req.getHost());
	LocBlock loc = serv.findLocBlock(req.getPath());
	return loc;
}

static bool isExistFile(std::string &filePath) {
	std::ifstream file(filePath.c_str());
	return file.good();
}

static std::string getErrorPage(LocBlock &loc, int status_code) {
	std::vector<int> error_codes = loc.getErrorCode();
	std::vector<int>::iterator it = std::find(error_codes.begin(), error_codes.end(), status_code);	
	if (it != error_codes.end())
		return loc.getCombineErrorPath();
	return "";
}

// static void handleHttpError() {
// 	if (res.getFilePath() == "")
// 		res.processDefaultErrorRes(res.getStatusCode());
// 	res.processDefaultErrorRes(res.getStatusCode());
// }

void HttpResponse::processErrorRes(int status_code) {
	status_code_ = 404;
	file_path_ = getErrorPage(loc_block_, status_code_);
	res_type_ = ERROR;
	processDefaultErrorRes(status_code);
}



/**
 * @brief 반환할 ErrorLocation이 없는 경우에 사용
 * 
 * @param res 채워질 HttpResponse 객체
 * @param status_code 
 */
void HttpResponse::processDefaultErrorRes(int status_code) {
	status_code_ = status_code;
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
	std::cout << std::string(joined_data_.begin(), joined_data_.end()) << "\n"; 
}


void HttpResponse::processRedirectRes(int status_code) {
	status_code_ = status_code;
	status_ = status_store_[status_code_];
	
	std::string header =
	http_version_ + " " + status_ + "\r\n" +
	"Location: " + location_ + "\r\n\r\n";
	
	joined_data_.insert(joined_data_.end(), header.begin(), header.end()); // insert 말고 덮어써야함
	std::cout << std::string(joined_data_.begin(), joined_data_.end()) << "\n";
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
	if (file_path_ != "") {
		res_type_ = REDIRECT;
		location_ = file_path_;
		// HttpMethod::processRedirectRes(301);
		return;
	}
	file_path_ = loc.getCombineCgiPath();
	if (file_path_ != "") {
		res_type_ = CGI;
		Cgi::forkCgi(req);
		return;
	}
	file_path_ = loc.getCombineUploadStorePath();
	if (file_path_ != "") {
		res_type_ = UPLOAD_STORE;
		// HttpMethod::handleHttpMethod(req, *this);
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

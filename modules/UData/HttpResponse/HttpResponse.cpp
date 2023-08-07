#include "HttpResponse.hpp"

static LocBlock &initLocBlock(HttpRequest &req) {
	ServBlock serv = ConfParser::getInstance().getServBlock(req.getPort(), req.getHost());
	LocBlock loc = serv.findLocBlock(req.getPath());
	return loc;
}

static bool isExistFile(std::string &filePath) {
	std::ifstream file(filePath.c_str());
	return file.good();
}

void HttpResponse::setFilePath(HttpRequest &req, LocBlock &loc) {
	file_path_ = loc.getCombineReturnPath();
	if (file_path != "") {
		res_type_ = REDIRECT;
		location_ = file_path_;
		Redirect::processRedirectRes(res, 301);
		return;
	}
	file_path_ = loc.getCombineCgiPath();
	if (file_path_ != "") {
		res_type_ = CGI;
		CGI::forkCgi(req);
		return;
	}
	file_path_ = loc.getCombineUploadStorePath();
	if (file_path_ != "" && isExistFile(file_path_)) {
		res_type_ = UPLOAD_STORE;
		HttpMethod::handleHttpMethod(res);
	}
	else {
		res_type_ = ERROR;
		status_code_ = 404;
		std::vector<int> error_codes = loc.getErrorCode();
		std::vector<int>::iterator it = std::find(numbers.begin(), numbers.end(), 404);
		if (it != numbers.end())
			file_path_ = getCombineErrorPath();
		else
			file_path_ = "";
		HttpMethod::handleHttpMethod(res);
	}
	return;
}

static std::string &initLocation(LocBlock &loc) {
//	return location;
}

HttpResponse::HttpResponse(HttpRequest &req) : http_version_("HTTP/1.1"),  status_code_(200), status_(""), content_length_(0), content_type_(""), location_(""),  loc_block_(initLocBlock(req)), res_type_(UPLOAD_STORE), file_path_("") {
	setFilePath(req, loc_block_);
	
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

const std::vector<char>& HttpResponse::getJoinedData()const{return joined_data_;}

int main() {
	HttpResponse res;
	res.initStatusStore();
	res.processDefaultErrorRes(res, 404);
	res.processRedirectRes(res, 301);
	return 0;
}
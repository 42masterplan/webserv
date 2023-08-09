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
  setFileSize(file_path_);
}

/* init */

bool HttpResponse::isExistFile(std::string &filePath) {
	std::ifstream file(filePath.c_str());
	return file.good();
}

void HttpResponse::processErrorRes(int status_code) {
	status_code_ = status_code;
	res_type_ = ERROR;
	file_path_ = getErrorPath(status_code);
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

void	HttpResponse::makeBodyResponse(int status_code, int content_length){
	status_code_ = status_code;
	status_ = status_store_[status_code_];
	std::string header =
	http_version_ + " " + status_ + "\r\n"+
	"content_length: " + std::to_string(content_length) + "\r\n\r\n";
	joined_data_.clear();
	joined_data_.insert(joined_data_.end(), header.begin(), header.end());
}


std::string HttpResponse::getErrorPath(int status_code){
	std::vector<int> error_codes = loc_block_.getErrorCode();
	std::vector<int>::iterator it = std::find(error_codes.begin(), error_codes.end(), status_code);	
	if (it != error_codes.end())
		return loc_block_.getCombineErrorPath();
	return std::string(DEFAULT_ERROR_PATH);
}

/* getter, setter */
void HttpResponse::setStatusCode(int status_code) { status_code_ = status_code; }
std::vector<char> &HttpResponse::getBody() { return body_; }
const std::string &HttpResponse::getFilePath() const { return file_path_; }

bool HttpResponse::isFolder(const std::string& file_path_) const {
  struct stat path_info;
  if (stat(file_path_.c_str(), &path_info) != 0)
    throw std::runtime_error("stat() ERROR");
  if (S_ISDIR(path_info.st_mode))
    return true;
  return false;
}

void  HttpResponse::setFileSize(const std::string& file_path_) {
  struct stat file_stat;
  if (file_path_ == ""){
    file_size_ = 0; //TODO: -1로 초기화할까 합니다.
    return;
  }
  if (stat(file_path_.c_str(), &file_stat) != 0)
    throw std::runtime_error("stat() ERROR");
  file_size_ = file_stat.st_size;
}

static bool isUploadMethod(HttpRequest &req) {
	const e_method method = req.getMethod();
	if (method == POST || method == PUT || method == PATCH)
		return true;
	return false;
}

void HttpResponse::setFilePath(HttpRequest &req, LocBlock &loc) {
	file_path_ = loc.getCombineReturnPath();
	if (file_path_ != "") {
		res_type_ = REDIRECT;
		location_ = file_path_;
		processRedirectRes(loc.getReturnCode());//여기서 첫번째 줄과 헤더 합쳐서 메세지 다 만들어서 joined_data_에 넣어줍니다.
		return; // 4 분기문 전부 processRes 여기서 하거나 밖에서 하거나 통일 좀 해야겠다
	}
  if (loc.isAutoIndex() && isFolder(loc.getCombineLocPath())){
    res_type_ = AUTOINDEX;
    return;
  }
	file_path_ = loc.getCombineCgiPath();
	if (file_path_ != ""){
		res_type_ = CGI_EXEC;
		return;
	}
	file_path_ = loc.getCombineUploadStorePath();
	if (isUploadMethod(req) && file_path_ == ""){//upload 하려고 하는데 그 경로가 설정파일에서 없으면 서버에러가 아니고 잘못된 요청
		res_type_ = ERROR;
		processErrorRes(404);
		return;
	}
}

std::vector<char>& HttpResponse::getJoinedData(){ return joined_data_; }

// int main() {
// 	HttpResponse res;
// 	res.initStatusStore();
// 	res.processDefaultErrorRes(res, 404);
// 	res.processRedirectRes(res, 301);
// 	return 0;
// }

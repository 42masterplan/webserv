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
	exist_session_ = ref.exist_session_;
	body_ = ref.body_;
	joined_data_ = ref.joined_data_;
	res_type_ = ref.res_type_;
	file_path_ = ref.file_path_;
	client_fd_ = ref.client_fd_;
	write_size_ = ref.write_size_;
	loc_block_ = ref.loc_block_;
	return *this;
}

HttpResponse::HttpResponse(HttpRequest &req) : http_version_("HTTP/1.1"), status_code_(200), status_(""), content_length_(0), content_type_(""), location_(""), exist_session_(req.getExistSession()), loc_block_((ConfParser::getInstance().getServBlock(req.getPort(), req.getHost())).findLocBlock(req.getPath())), res_type_(METHOD_TYPE), file_path_("") {
  try{
		// loc_block_.printInfo();
		setFilePath(req, loc_block_);
		if (req.getMethod() == GET || req.getMethod() == HEAD)
			setFileSize(file_path_);
  }catch(std::exception& e){ //이곳은 isFolder에서 throw된 예외가 잡힙니다. 이 경우 존재하지 않는 폴더 혹은 파일의 요청입니다.
		std::cout << "Response 생성자에서 에러 발생!!" <<std::endl;
    res_type_ = ERROR;
		processErrorRes(404);
  }
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

	status_ = status_msg_store_.getStatusMsg(status_code_);
	// status_ = status_store_[status_code_];
	
	std::string header =
	http_version_ + " " + status_ + "\r\n" +
	"Location: " + location_ + "\r\n\r\n";
	
	joined_data_.clear();
	joined_data_.insert(joined_data_.end(), header.begin(), header.end());
	// std::cout << std::string(joined_data_.begin(), joined_data_.end()) << "\n";
}


void	HttpResponse::makeBodyResponse(int status_code, int content_length){
	std::string	header = "";

	status_code_ = status_code;
	status_ = status_msg_store_.getStatusMsg(status_code_);
	// std::cout << "여기 왔다~~"<<std::endl;

	header += http_version_ + " " + status_ + "\r\n";

	// TODO: MimeStore 사용하기
	if (status_code_ == 201 || (status_code_ >= 300 && status_code_ < 400))
		header += "Location: " + location_ + "\r\n";
	if (status_code_ == 405 && loc_block_.getDenyMethod().size()) {
		std::vector<std::string> allow_method = loc_block_.getDenyMethod();
		header += "Allow: ";
		for (size_t i = 0; i < allow_method.size(); i++) {
			header += allow_method[i];
			if (i != allow_method.size() - 1)
				header += ", ";
		}
		header += "\r\n";
	}
	if ((status_code_ >= 200 && status_code_ < 400) && !exist_session_)
		header += "Set-Cookie: SESSIONID=" + Session::getInstance().createSession() + "\r\n";
	header += "Content-Type: text/html; charset=utf-8\r\n";
	header += "Content-Length: " + std::to_string(content_length) + "\r\n\r\n";
	
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

void  HttpResponse::setFileSize(const std::string& file_path_) {
  struct stat file_stat;
  if (file_path_ == ""){
    file_size_ = -1;
    return;
  }
  if (stat(file_path_.c_str(), &file_stat) != 0)
    throw std::runtime_error("stat() ERROR");
	// std::cout << "파일 크기 만드는중 ~~" << file_stat.st_size <<std::endl;
  file_size_ = static_cast<long>(file_stat.st_size);
}

static bool isUploadMethod(HttpRequest &req) {
	const e_method method = req.getMethod();
	if (method == POST || method == PUT || method == PATCH)
		return true;
	return false;
}

void HttpResponse::setFilePath(HttpRequest &req, LocBlock &loc) {
	file_path_ = loc.getReturnPath();
	if (file_path_ != "") {
		res_type_ = REDIRECT;
		location_ = file_path_;
		return; // 4 분기문 전부 processRes 여기서 하거나 밖에서 하거나 통일 좀 해야겠다
	}
	file_path_ = loc.getCombineCgiPath();
	if (file_path_ != ""){
		res_type_ = CGI_EXEC;
		return;
	}
	file_path_ = loc.getCombineUploadStorePath();
	if (isUploadMethod(req)){//upload 하려고 하는데 그 경로가 설정파일에서 없으면 서버에러가 아니고 잘못된 요청
		if (file_path_ == ""){
			res_type_ = ERROR;
			processErrorRes(404);
			return ;
		}
		file_path_ +=  loc.getFileName();
		return;
	}
	file_path_ = loc.getCombineLocPath();
	// std::cout <<"file -------"<< req.getPath()<< "|" << file_path_ << std::endl;
  if (loc.isAutoIndex()){
	 struct stat path_info;
		
    if (stat(loc.getCombineLocPath().c_str(), &path_info) != 0){
			std::cout << "어째서 여기!"<<std::endl;
			throw(std::runtime_error("STAT ERROR()"));
		}
		if (isFolder(loc.getCombineLocPath()) == true)
			res_type_ = AUTOINDEX;
    return;
  }
}

void	HttpResponse::setLocation(std::string location) { location_ = location; }


std::vector<char>& HttpResponse::getJoinedData(){ return joined_data_; }

// int main() {
// 	HttpResponse res;
// 	res.initStatusStore();
// 	res.processDefaultErrorRes(res, 404);
// 	res.processRedirectRes(res, 301);
// 	return 0;
// }
